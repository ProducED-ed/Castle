try:
        # 1. Освобождение портов
        if board_id == 'dog':
            socketio.emit('flash_log', {'board': board_id, 'msg': 'Отправка команды release_serial3 на Главную плату...\n'})
            serial_write_queue.put('release_serial3')
            eventlet.sleep(1.5)
        elif board_id == 'main':
            is_system_flashing = True
            eventlet.sleep(1)
            try: ser.close()
            except: pass

        socketio.emit('flash_log', {'board': board_id, 'msg': 'Запуск прошивки (это может занять около минуты)...\n'})
        
        # 2. Выполнение консольной команды
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        out_msg = result.stdout + '\n' + result.stderr
        
        # Печатаем лог в терминал
        socketio.emit('flash_log', {'board': board_id, 'msg': out_msg})
        
        # Печатаем скрытый системный код ответа (0 - успех, всё остальное - ошибка)
        socketio.emit('flash_log', {'board': board_id, 'msg': f'\n[DEBUG] Системный код завершения: {result.returncode}\n'})
        
        # 3. ИДЕАЛЬНАЯ ПРОВЕРКА НА УСПЕХ
        lower_out = out_msg.lower()
        is_success = False
        
        # Разделяем логику успеха для кабеля и воздуха
        if board_id in ['main', 'dog', 'owls', 'basket', 'workshop']:
            # Для Arduino (avrdude) - ищем фразу о проверке памяти
            if "bytes of flash verified" in lower_out:
                is_success = True
        else:
            # Для ESP32 (espota.py) - верим нулевому коду возврата системы, 
            # даже если скрипт не успел написать "Success" из-за ребута платы
            if result.returncode == 0:
                is_success = True
            elif "result: ok" in lower_out or "success" in lower_out:
                is_success = True

        # 4. Обработка результатов
        if is_success:
            now_str = datetime.now().strftime('%d.%m.%Y %H:%M')
            flashes = {}
            if os.path.exists(FLASH_STATS_FILE):
                with open(FLASH_STATS_FILE, 'r') as f: flashes = json.load(f)
            flashes[board_id] = now_str
            with open(FLASH_STATS_FILE, 'w') as f: json.dump(flashes, f)
            
            socketio.emit('flash_success', {'board': board_id, 'time': now_str})
            socketio.emit('flash_log', {'board': board_id, 'msg': '\n[SYSTEM] ОШИБОК НЕ ОБНАРУЖЕНО. Прошивка успешна!\n'})
        else:
            socketio.emit('flash_failed', {'board': board_id})
            socketio.emit('flash_log', {'board': board_id, 'msg': '\n[SYSTEM] ВНИМАНИЕ! Обнаружена ошибка. Плата НЕ прошилась!\n'})

        # 5. Восстановление портов
        if board_id == 'dog':
            socketio.emit('flash_log', {'board': board_id, 'msg': 'Восстановление связи (restore_serial3)...'})
            serial_write_queue.put('restore_serial3')
            eventlet.sleep(0.5)
            socketio.emit('flash_log', {'board': board_id, 'msg': '\nСвязь успешно восстановлена! [END]'})
        elif board_id == 'main':
            try: ser.open()
            except: pass
            is_system_flashing = False
            socketio.emit('flash_log', {'board': board_id, 'msg': '\nСервер возобновил работу. [END]'})
        else:
            socketio.emit('flash_log', {'board': board_id, 'msg': '\n[END]'})

    except Exception as e:
        socketio.emit('flash_failed', {'board': board_id})
        socketio.emit('flash_log', {'board': board_id, 'msg': f'\n[SYSTEM] КРИТИЧЕСКАЯ ОШИБКА: {str(e)} [END]'})
        is_system_flashing = False
        try: ser.open()
        except: pass
