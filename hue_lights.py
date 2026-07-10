"""
Управление лампами Philips Hue из CLC сервера.
Синхронный клиент. Из CastleServer.py вызывается через eventlet.spawn_n,
чтобы не блокировать главный цикл.

Модель управления — ПРЯМАЯ ЯРКОСТЬ выбранной ГРУППЫ Hue (напр. "Castle"):
    set_light(on=True, bri_pct=100)  → группа включается на 100%
    set_light(on=True, bri_pct=40)   → 40% яркости
    set_light(on=False)              → выключить
Это даёт точную яркость под сценарий квеста (в отличие от фиксированных сцен).

Состояние — в hue_config.json рядом с CastleServer.py:
    {
      "bridge_ip": "192.168.1.141",
      "app_key":  "<получается при pairing>",
      "enabled":  true/false,     # общий тумблер фичи на /tech
      "group_id": "2"             # какой группой Hue управлять (напр. "Castle")
    }
"""
import json
import logging
import os
import requests

logger = logging.getLogger(__name__)

DEFAULT_TIMEOUT = 2.0
DEVICETYPE = "clc_castle_quest#raspberrypi"


class HueClient:
    def __init__(self, config_path, default_ip="192.168.1.141", default_group="2"):
        self.config_path = config_path
        self.bridge_ip = default_ip
        self.app_key = None
        self.enabled = False
        self.group_id = default_group
        self._group_names = {}       # кэш {id: name} для читаемых логов
        self._group_light_ids = []   # кэш id ламп в текущей группе
        self.load()

    # ---------- persistence ----------
    def load(self):
        if not os.path.exists(self.config_path):
            return
        try:
            with open(self.config_path, "r") as f:
                data = json.load(f)
            self.bridge_ip = data.get("bridge_ip", self.bridge_ip)
            self.app_key = data.get("app_key")
            self.enabled = bool(data.get("enabled", False))
            self.group_id = str(data.get("group_id", self.group_id))
            logger.info(f"HUE: config loaded (ip={self.bridge_ip}, "
                        f"paired={bool(self.app_key)}, enabled={self.enabled}, "
                        f"group={self.group_id})")
        except Exception as e:
            logger.warning(f"HUE: cannot load {self.config_path}: {e}")

    def save(self):
        try:
            with open(self.config_path, "w") as f:
                json.dump({
                    "bridge_ip": self.bridge_ip,
                    "app_key": self.app_key,
                    "enabled": self.enabled,
                    "group_id": self.group_id,
                }, f, indent=2)
            logger.info(f"HUE: config saved to {self.config_path}")
        except Exception as e:
            logger.warning(f"HUE: cannot save {self.config_path}: {e}")

    # ---------- setters (from /tech UI) ----------
    def set_bridge_ip(self, ip):
        self.bridge_ip = (ip or "").strip()
        self.app_key = None  # смена IP аннулирует старый app_key
        self.save()

    def set_enabled(self, on):
        self.enabled = bool(on)
        self.save()

    def set_group(self, group_id):
        self.group_id = str(group_id) if group_id else self.group_id
        self.save()

    # ---------- state ----------
    def is_paired(self):
        return bool(self.app_key)

    def is_enabled(self):
        return self.enabled

    def _group_name(self, gid=None):
        gid = gid if gid is not None else self.group_id
        return self._group_names.get(str(gid), str(gid))

    def status(self):
        return {
            "bridge_ip": self.bridge_ip,
            "paired": self.is_paired(),
            "enabled": self.enabled,
            "app_key_short": (self.app_key[:8] + "...") if self.app_key else None,
            "group_id": self.group_id,
            "group_name": self._group_name(),
        }

    # ---------- pairing ----------
    def try_pair_once(self):
        """Одна попытка pairing. Пока клиент не нажал кнопку — бридж вернёт
        error 101 'link button not pressed'. Возвращает (success, message)."""
        try:
            r = requests.post(
                f"http://{self.bridge_ip}/api",
                json={"devicetype": DEVICETYPE},
                timeout=DEFAULT_TIMEOUT,
            )
            data = r.json()
            if isinstance(data, list) and data:
                item = data[0]
                if "success" in item:
                    self.app_key = item["success"]["username"]
                    self.save()
                    return True, "paired"
                if "error" in item:
                    return False, item["error"].get("description", "unknown error")
            return False, f"unexpected response: {data}"
        except Exception as e:
            return False, f"exception: {e}"

    # ---------- queries ----------
    def _get(self, path):
        if not self.app_key:
            return None
        try:
            r = requests.get(
                f"http://{self.bridge_ip}/api/{self.app_key}/{path}",
                timeout=DEFAULT_TIMEOUT,
            )
            return r.json()
        except Exception as e:
            logger.warning(f"HUE GET {path} failed: {e}")
            return None

    def get_lights(self):
        data = self._get("lights")
        if isinstance(data, dict):
            return {lid: info.get("name", "?") for lid, info in data.items()}
        return {}

    def get_groups(self):
        """{group_id: {name, lights}}. Побочно кэшируем имена для логов."""
        data = self._get("groups")
        if isinstance(data, dict):
            names = {}
            out = {}
            for gid, info in data.items():
                names[gid] = info.get("name", "?")
                out[gid] = {"name": info.get("name", "?"),
                            "lights": info.get("lights", [])}
            self._group_names = names
            return out
        return {}

    # ---------- actuation ----------
    def _put_group_action(self, body):
        """PUT в текущую группу. Логирует на DEBUG — понятный INFO пишется в set_light()."""
        if not self.app_key:
            return False
        try:
            r = requests.put(
                f"http://{self.bridge_ip}/api/{self.app_key}/groups/{self.group_id}/action",
                json=body,
                timeout=DEFAULT_TIMEOUT,
            )
            logger.debug(f"HUE raw action group={self.group_id} {body} → HTTP {r.status_code}")
            return r.status_code == 200
        except Exception as e:
            logger.warning(f"HUE action {body} failed: {e}")
            return False

    def set_light(self, on, bri_pct=100):
        """Свет включить на заданной яркости (0..100%) или выключить.
        Управляет группой self.group_id (напр. 'Castle')."""
        if not on:
            body = {"on": False}
            ok = self._put_group_action(body)
            logger.info(f"HUE: свет ВЫКЛючить (группа '{self._group_name()}') "
                        f"→ {'OK' if ok else 'ОШИБКА'}")
            return ok
        pct = max(1, min(100, int(bri_pct)))
        bri = max(1, min(254, round(pct / 100 * 254)))  # Hue bri: 1..254
        body = {"on": True, "bri": bri}
        ok = self._put_group_action(body)
        logger.info(f"HUE: свет ВКЛючить {pct}% (группа '{self._group_name()}') "
                    f"→ {'OK' if ok else 'ОШИБКА'}")
        return ok

    # ---------- per-lamp (для «1 лампа» и светомузыки) ----------
    def get_group_light_ids(self):
        """Список id ламп в текущей группе (напр. ['3','4','5']). Кэшируется."""
        if self._group_light_ids:
            return self._group_light_ids
        groups = self.get_groups()  # побочно кэширует имена групп
        info = groups.get(str(self.group_id))
        if info:
            self._group_light_ids = info.get("lights", [])
        return self._group_light_ids

    def _put_light_state(self, light_id, body):
        if not self.app_key:
            return False
        try:
            r = requests.put(
                f"http://{self.bridge_ip}/api/{self.app_key}/lights/{light_id}/state",
                json=body,
                timeout=DEFAULT_TIMEOUT,
            )
            return r.status_code == 200
        except Exception as e:
            logger.warning(f"HUE light {light_id} {body} failed: {e}")
            return False

    def _bri_from_pct(self, bri_pct):
        pct = max(1, min(100, int(bri_pct)))
        return max(1, min(254, round(pct / 100 * 254)))

    def set_single_light(self, light_id, on, bri_pct=100):
        """Одна конкретная лампа вкл/выкл (для сцены «горит только 1 лампа»)."""
        if not on:
            return self._put_light_state(light_id, {"on": False})
        return self._put_light_state(light_id, {"on": True, "bri": self._bri_from_pct(bri_pct)})

    def set_light_color(self, light_id, hue_val, sat, bri_pct=100, transition_ms=200):
        """Цвет одной лампы (для светомузыки). hue 0..65535, sat 0..254."""
        return self._put_light_state(light_id, {
            "on": True,
            "hue": int(hue_val) % 65536,
            "sat": max(0, min(254, int(sat))),
            "bri": self._bri_from_pct(bri_pct),
            "transitiontime": max(0, int(transition_ms / 100)),
        })
