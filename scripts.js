//в файле скриптов мы управляем нашей разметкой пишем логику  
$(document).ready(function(){ 
  /////////////////////////////////////////// переменные так же определяем с какого устройства подключился к серверу клиент
  var mobile = /Android|webOS|iPhone|iPad|iPod|BlackBerry/i.test(navigator.userAgent);
  var start = mobile ? "touchstart" : "mousedown";//если подключились с мобилки меняем метод mousedown на touchstart
  var stop = mobile ? "touchend" : "mouseup"; //так же меняет методы mouseup на touchend
  var leave = mobile ? "touchend" : "mouseleave";//mouseleave на touchend
  var timeRate = '';
  var rate = '';
  var count_help = '';
  var lev1 = 0
  var lev2 = 0
  var lev3 = 0
  var lev4 = 0
  var lev5 = 0
  var lev6 = 0
  var lev7 = 0
  var lev11 = 0
  var lev12 = 0
  var lev13 = 0
  var lev14 = 0
  var lev15 = 0
  var lev16 = 0
  var lev17 = 0
  let arr = [];
 //присваеваем кнопкам громкости переменные и задаем для них таймеры нужны для длительного нажатия
  var timerIntervalDownEffect = 0,
      buttonDownEffect = document.getElementById("effectDown");
  var timerIntervalUpEffect,
      buttonUpEffect = document.getElementById("effectUp");

  var timerIntervalDownPhone = 0,
      buttonDownPhone = document.getElementById("phoneDown");
  var timerIntervalUpPhone,
      buttonUpPhone = document.getElementById("phoneUp");

  var timerIntervalDownVoice = 0,
      buttonDownVoice = document.getElementById("voiceDown");
  var timerIntervalUpVoice,
      buttonUpVoice = document.getElementById("voiceUp");

  var timerIntervalDownWolf = 0,
      buttonDownWolf = document.getElementById("wolfDown");
  var timerIntervalUpWolf,
      buttonUpWolf = document.getElementById("wolfUp");    

    var timerIntervalDownPlatform = 0,
      buttonDownPlatform = document.getElementById("platformDown");
  var timerIntervalUpPlatform,
      buttonUpPlatform = document.getElementById("platformUp");  

  var timerIntervalDownSuitcases= 0,
      buttonDownSuitcases = document.getElementById("suitcasesDown");
  var timerIntervalUpSuitcases,
      buttonUpSuitcases = document.getElementById("suitcasesUp");    
      
 var timerIntervalDownSafe= 0,
      buttonDownSafe = document.getElementById("safeDown");
  var timerIntervalUpSafe,
      buttonUpSafe = document.getElementById("safeUp");       
    
     //добавляем всем кнопкам класс basic нужен просто для внешки
    $('.button').addClass('basic');
    $('#Start').css('border','grey 2px solid');
    $('#start_icon').removeClass('green');
    $('#start_icon').addClass('grey');
    $('#Start').addClass('disabled');
    //переключение тем метод removeClass удаляет нужный нам класс
       
    $('#white').click(function(){
        
        $('.ui.center.aligned.header').css('color','black');
        $('.ui').removeClass('inverted');
                $('.page').css('background-color','white');
                $('.timers').css('background-color','white');
                $('.timers').css('border','white 2px solid');
                $('.sett').css('background-color','white');
                $('#teext').css('color','black');
                $('#volt').css('color', 'black');
                
            
    });
    $('#black').click(function(){
        $('.ui.center.aligned.header').css('color','white');
        $('.ui').addClass('inverted');
                $('.page').css('background-color','#191919');
                $('.timers').css('background-color','#191919');
                $('.timers').css('border','#191919 2px solid');
                $('.sett').css('background-color','#191919');
                $('#teext').css('color','white');
                $('#volt').css('color', 'white');
                
                
    });

    
    //тут настраиваются наши socket
    var output;
var socket;
var intID = undefined;
var restflag = 0;
var connected = false;
var disBut = 0;
//эти скрипты должны быть что бы работало боковое меню и липкое меню и выпадающие списки без них работать не будет
$('#first').click(function(){
$('.ui.sidebar')
.sidebar('toggle')
;
});
$('.ui.dropdown')
.dropdown()
;
  $('.ui.sticky')
.sticky({
  context: '.page'
});


//флаги
     var rFlag = 0;
     var gFlag=0;
     var yFlag=0;
     var bFlag=0;
     var bottle1 = 0;
     var bottle2 =0;
     var bottle3 = 0;
     var bottle4=0;
     var goal1player=0;
     var goal2player=0;
     var goal3player=0;
     var goal4player=0;
     var goal5player=0;
     var goal6player=0;
     var goal7player=0;
     var goal8player=0;
     var goal9player=0;
     var goal10player=0;
     var goal1bot=0;
     var goal2bot=0;
     var goal3bot=0;
     var goal4bot=0;
     var goal5bot=0;
     var goal6bot=0;
     var goal7bot=0;
     var goal8bot=0;
     var goal9bot=0;
     var goal10bot=0;
     var crystal4On =1;
     var crystal1On =1;
     var crystal2On =1;
     var crystal3On =1;
     var bugAlert = 0;
     var modal=0;
     var start_error = 1;
     var devices = "";
     
     // Инициализация прогресс-баров ---
     $('#safe_progress').progress({
        percent: 0
     });
     $('#troll_progress').progress({
        percent: 0
     });
     $('#ghost_progress').progress({
        percent: 0
     });
     $('#owls_progress').progress({
        percent: 0
     });
	 $('#spell_progress').progress({
        percent: 0
     });

 //настройка подключения не менять
     output = document.getElementById('output');//присвоим переменной значение элемента с id output 
     socket = io.connect('http://' + document.domain + ':' + location.port);//строка подключения сокетов
	 //socket = io.connect('http://127.0.0.1:3000');
     socket.on('connect', function() {//если наш сервер подключен он отправит сообщение коннектед
         output.innerHTML = 'Connected';// выводим текст conneted
         connected = true;
         $('#output').transition('remove looping')//уберем пульсацию если было в состоянии disconnected
         setTimeout(function() {
             if (connected) {
                 output.innerHTML = '0:00:00';// если в 2 секунды свзяь не потерялась выводим текст с нашим таймером
             }					
         }, 3000);
         intID = setInterval(function() {
             socket.emit('Game', {'level': 'all'});//отправляем heartbeat опрашиваем сервер раз в 100 мс
             //socket.emit('time', 'timer');    
              
             
         }, 100);
         
         
     });
     //если нет ответа от сервака 
     socket.on('disconnect', function() {
         output.innerHTML = 'Disconnected';//выводим слово disconnected
         $('#output')//делаем анимацию пульсирования бесконечного раз в секунду
    .transition('set looping')
    .transition({
animation  : 'pulse',
duration   : '1s',
});
         connected = false;
         if (intID) {
             intID = clearInterval(intID);//обновляет интервал для того что бы когда подключишься и 2 секунды связь не потеряется то можем считать что сервер работает
         }				
     });
    //тут начинается логика которую можно менять первые 3 метода сокетов принимают данные от сервера по уровню громкости каналов
    //настройка для канала с голосом
    socket.on('volume2', function (v) {
        //приводим строку в число 
        lev3 = Number(v)
        //приходит в десятичном ввиде округляем и умножаем на 100
        lev13 = lev3.toFixed(2) * 100
        //выводим значение еще раз округлив на всякий
        console.log(Math.floor(lev13))
        $('#voiceCount').text(Math.floor(lev13));
    });
    socket.on('wolf', function (v) {
        //приводим строку в число 
        lev4 = Number(v)
        //приходит в десятичном ввиде округляем и умножаем на 100
        lev14 = lev4
        //выводим значение еще раз округлив на всякий
        console.log(lev14)
        $('#wolfCount').text(lev14);
    });
    socket.on('platform', function (v) {
        //приводим строку в число 
        lev5 = Number(v)
        //приходит в десятичном ввиде округляем и умножаем на 100
        lev15 = lev5
        //выводим значение еще раз округлив на всякий
        console.log(lev15)
        $('#platformCount').text(lev15);
    });
    socket.on('suitcases', function (v) {
        //приводим строку в число 
        lev6 = Number(v)
        //приходит в десятичном ввиде округляем и умножаем на 100
        lev16 = lev6
        //выводим значение еще раз округлив на всякий
        console.log(lev16)
        $('#suitcasesCount').text(lev16);
    });

    socket.on('safe', function (v) {
        //приводим строку в число 
        lev7 = Number(v)
        //приходит в десятичном ввиде округляем и умножаем на 100
        lev17 = lev7
        //выводим значение еще раз округлив на всякий
        console.log(lev17)
        $('#safeCount').text(lev17);
    });
    //настройка для канала с эффектами
    socket.on('volume1', function (v) {

        lev2 = Number(v)
        lev12 = lev2.toFixed(2) * 100
        console.log(Math.floor(lev12))
        $('#effectCount').text(Math.floor(lev12));

    });
    //канал для работы с фоном логика одна и та же
    socket.on('volume', function (z) {
        lev1 = Number(z)
        lev11 = lev1.toFixed(2) * 100;
        console.log(Math.floor(lev11))
        $('#phoneCount').text(Math.floor(lev11));
    });
    //данный метод принимает значение напряжения 
    socket.on('volt', function (v) {
        //проверим если пришло значение строковое то выводим
        if (typeof (v) === 'string') {
            //выводим в доп меню через селектор в виде id
            $('#v').text(v);

        }
    });
    // здесь принимает строку времени выводим в главном меню
    socket.on('timer', function (e) {
        //проверка на строку
        if (typeof (e) === 'string') {
            $('#output').text(e);

        }
    });
    //получаем значение рейтинга и времен
    socket.on('rate', function (timers) {
        if (typeof (timers) === 'string') {
            //т.к. присылаются периодично перезаписываем в переменную
            timeRate = timers;
        }
    });
    //----аналогично
    socket.on('hintCount', function (h) {
        if (typeof (h) === 'string') {
            count_help = h;
        }
    });
    socket.on('rating', function (r) {
        if (typeof (r) === 'string') {
            rate = r;
        }
    });
     socket.on('devices', function (r) {
        if (typeof (r) === 'string') {
            devices = r;
        }
    });
    //нажатие на паузу отправляет данные сперва на сервер потом если ответ с сервера придет что кнопка обработана она окраситься в желтый см ниже
    $('#Pause').click(function(){
        socket.emit('time', 'pause');
    }); 
    //нажали на демо отправили на сервер сообщение
    $('#Ready').click(function(){
        socket.emit('time', 'ready');
        start_error = 1
    }); 
    //нажатие на старт тоже отправляем данные на сервак и отпускаем флаги
    $('#Start').click(function(){
      socket.emit('time', 'start');//шлем на сервер
      rFlag = 0;
      gFlag=0;
      yFlag=0;
      bFlag=0;
      incCombo1 = 0;
      incCombo2 = 0;
      incCombo3 = 0;
      incCombo4 = 0;
      bottle1 = 0;
      bottle2 =0;
      bottle3 = 0;
      bottle4=0;
      goal1player=0;
      goal2player=0;
      goal3player=0;
      goal4player=0;
      goal5player=0;
      goal6player=0;
      goal7player=0;
      goal8player=0;
      goal9player=0;
      goal10player=0;
      goal1bot=0;
      goal2bot=0;
      goal3bot=0;
      goal4bot=0;
      goal5bot=0;
      goal6bot=0;
      goal7bot=0;
      goal8bot=0;
      goal9bot=0;
      goal10bot=0;
      owl1 = 0;
      owl2 = 0;
      owl3= 0;
      owl4 = 0;
      item1 = 0;
      item2 = 0;
      item3 = 0;
      item4 = 0;
      item5 = 0;
      item6 = 0;
      scroll1 = 0;
      scroll2 = 0;
      scroll3 = 0;
      scroll4 = 0;
      scroll5 = 0;
      crystal4On =1;
      crystal1On =1;
      crystal2On =1;
      crystal3On =1;
      bugAlert = 0;
      modal=0;
      finalAlert = 1;
      start_error = 1;
    });
    //нажали на рестарт обрати внимание данные шлем на сервер пока на пульте ничего не отображается
    $('#Restart').click(function(){
      socket.emit('time', 'restart'); 
      rFlag = 0;
      gFlag=0;
      yFlag=0;
      bFlag=0;
      incCombo1 = 0;
      incCombo2 = 0;
      incCombo3 = 0;
      incCombo4 = 0;
      bottle1 = 0;
      bottle2 =0;
      bottle3 = 0;
      bottle4=0;
      goal1player=0;
      goal2player=0;
      goal3player=0;
      goal4player=0;
      goal5player=0;
      goal6player=0;
      goal7player=0;
      goal8player=0;
      goal9player=0;
      goal10player=0;
      goal1bot=0;
      goal2bot=0;
      goal3bot=0;
      goal4bot=0;
      goal5bot=0;
      goal6bot=0;
      goal7bot=0;
      goal8bot=0;
      goal9bot=0;
      goal10bot=0;
      crystal4On =1;
      crystal1On =1;
      crystal2On =1;
      //modal=0;
      //bugAlert = 0;
      crystal3On =1;
      owl1 = 0;
      owl2 = 0;
      owl3= 0;
      owl4 = 0;
      item1 = 0;
      item2 = 0;
      item3 = 0;
      item4 = 0;
      item5 = 0;
      item6 = 0;
      scroll1 = 0;
      scroll2 = 0;
      scroll3 = 0;
      scroll4 = 0;
      scroll5 = 0;
      start_error = 0

      //finalAlert = 0;
    });
    //здесь принимаем строки о прохождении отображаем на пульте
     socket.on('level', function(inp) {
        
        if (typeof(inp) === 'string') {
            console.log(inp);
            //пришло сообщение о модальном окне
            if (inp === 'modal') {
                if(modal==0){
                    //делаем под флагом что бы много раз не вылетало(шлет постоянно что бы на всех клиентах отобразилось)
                    swal.fire("Warning!", "Please check the doors!", "error");//говорим что бы закрыли все двери
                    modal =1;
                }
            }
            //закрываем окно автоматически как закрыли все двери
            if (inp === 'modal_end') {
                    swal.close();     
            }
			
			// Логика для иконок и прогресс-баров ---

            // 1. Workshop (Иконки)
            if (inp === 'helmet') {
                $('#helmet_icon').addClass('green');
            }
            if (inp === 'broom') {
                $('#broom_icon').addClass('green');
            }

            // 2. Safe (Прогресс-бар)
            if (inp === 'safe_step_1') {
                $('#safe_progress').progress({ percent: 20 });
            }
            if (inp === 'safe_step_2') {
                $('#safe_progress').progress({ percent: 40 });
            }
            if (inp === 'safe_step_3') {
                $('#safe_progress').progress({ percent: 60 });
            }
			if (inp === 'safe_step_4') {
                $('#safe_progress').progress({ percent: 80 });
            }
            if (inp === 'safe_end') { // 'safe_end' - это 5-й шаг
                $('#safe_progress').progress({ percent: 100 });
            }
            if (inp === 'safe_reset') {
                $('#safe_progress').progress({ percent: 0 });
            }

            // 3. Troll Game (Прогресс-бар)
            if (inp === 'cave_search1') { // aluminium
                $('#troll_progress').progress({ percent: 25 });
            }
            if (inp === 'cave_search2') { // bronze
                $('#troll_progress').progress({ percent: 50 });
            }
            if (inp === 'cave_search3') { // copper
                $('#troll_progress').progress({ percent: 75 });
            }
            if (inp === 'cave_end') { // 4-й шаг
                $('#troll_progress').progress({ percent: 100 });
            }

            // 4. Ghost Game (Прогресс-бар)
            if (inp === 'story_40') { // Шаг 1 (поезд)
                $('#ghost_progress').progress({ percent: 20 });
            }
            if (inp === 'story_41') { // Шаг 2 (волк)
                $('#ghost_progress').progress({ percent: 40 });
            }
            if (inp === 'story_42') { // Шаг 3 (поезд)
                $('#ghost_progress').progress({ percent: 60 });
            }
            if (inp === 'punch') { // Шаг 4 (финальный стук 'punch' -> 'stage_7')
                $('#ghost_progress').progress({ percent: 80 });
            }
            if (inp === 'set_time') { // Шаг 5 (подсказка 'star_hint' -> 'set_time')
                $('#ghost_progress').progress({ percent: 100 });
            }

            // 5. Owls (Прогресс-бар)
            if (inp === 'owl_flew_1') {
                $('#owls_progress').progress({ percent: 25 });
            }
            if (inp === 'owl_flew_2') {
                $('#owls_progress').progress({ percent: 50 });
            }
            if (inp === 'owl_flew_3') {
                $('#owls_progress').progress({ percent: 75 });
            }
            if (inp === 'owl_flew_4' || inp === 'owl_end') { // 'owl_end' также завершает
                $('#owls_progress').progress({ percent: 100 });
            }
			
			if (inp === 'spell_step_1') {
                $('#spell_progress').progress({ percent: 20 });
            }
            if (inp === 'spell_step_2') {
                $('#spell_progress').progress({ percent: 40 });
            }
            if (inp === 'spell_step_3') {
                $('#spell_progress').progress({ percent: 60 });
            }
            if (inp === 'spell_step_4') {
                $('#spell_progress').progress({ percent: 80 });
            }
            if (inp === 'spell_step_5') { // 'spell_step_5' - это 5-й шаг (успех)
                $('#spell_progress').progress({ percent: 100 });
            }
            if (inp === 'spell_reset') {
                $('#spell_progress').progress({ percent: 0 });
            }
			
           //тут у нас обрабатываются сообщения об октвации игр
           if (inp === 'active_first_clock') {
                $('#first_clock').removeClass('disabled');
            } 
            if (inp === 'active_second_clock') {
             $('#second_clock').removeClass('disabled');
            } 

            if (inp === 'active_first_clock_2') {
                $('#first_clock_2').removeClass('disabled');
            } 
            if (inp === 'active_second_clock_2') {
             $('#second_clock_2').removeClass('disabled');
            } 
            if (inp === 'active_open_mansard_door') {
                $('#open_mansard_door').removeClass('disabled');
            }
            if (inp === 'active_suitcase') {
                $('#suitcase').removeClass('disabled');
            }
            if (inp === 'active_animals') {
                $('#animals').removeClass('disabled');
            }
            if (inp === 'active_swolf') {
                $('#wolf').removeClass('disabled');
            }
            if (inp === 'active_open_mansard_stash') {
                $('#open_mansard_stash').removeClass('disabled');
            }
            if (inp === 'active_pedlock') {
                $('#pedlock').removeClass('disabled');
            }
            if (inp === 'active_dog') {
                $('#dog').removeClass('disabled');
            }
            if (inp === 'active_cat') {
                $('#cat').removeClass('disabled');
            }
            if (inp === 'active_open_potions_stash') {
                $('#open_potions_stash').removeClass('disabled');
            }
            if (inp === 'active_owl') {
                $('#owl').removeClass('disabled');
            }
            if (inp === 'active_owls') {
                $('#owls').removeClass('disabled');
            }
            if (inp === 'active_train') {
                $('#train').removeClass('disabled');
            }
            if (inp === 'active_mine') {
                $('#mine').removeClass('disabled');
            }
            if (inp === 'active_troll') {
                $('#troll').removeClass('disabled');
            }
            if (inp === 'active_open_bank_door') {
                $('#open_bank_door').removeClass('disabled');
            }
            if (inp === 'active_safe') {
                $('#safe').removeClass('disabled');
            }
            if (inp === 'active_workshop') {
                $('#workshop').removeClass('disabled');
            }
            if (inp === 'active_projector') {
                $('#projector').removeClass('disabled');
            }
            if (inp === 'active_ghost') {
                $('#ghost').removeClass('disabled');
            }
            if (inp === 'active_crime') {
                $('#crime').removeClass('disabled');
            }
            if (inp === 'active_cup') {
                $('#cup').removeClass('disabled');
            }
            if (inp === 'active_spell') {
                $('#spell').removeClass('disabled');
            }
            if (inp === 'active_crystals') {
                $('#crystals').removeClass('disabled');
            }
            if (inp === 'active_open_memory_stash') {
                $('#open_memory_stash').removeClass('disabled');
            }
            if (inp === 'active_basket') {
                $('#basket').removeClass('disabled');
            }
            if (inp === 'open_door') {//открыли стартовую дверь
                $('#first_door').addClass('open');// добавили класс open замок поменяеться на открытый
                $('#first_door').addClass('green');//добавив класс green окрасим в зеленый
            }  
            if (inp === 'close_door') {
                $('#first_door').removeClass('open');//удаляем класс open
                $('#first_door').removeClass('green');//удаляем класс зеленый
            }
            if (inp === 'open_door_puzzle') {
                $('#door_puzzle').addClass('open');//удаляем класс open
                $('#door_puzzle').addClass('green');
            }  
            if (inp === 'close_door_puzzle') {
                $('#door_puzzle').removeClass('open');//удаляем класс open
                $('#door_puzzle').removeClass('green');
            }
            if(inp === 'first_clock') {
                $('#first_clock').addClass('positive');//добавим кнопке класс postive окрасится в зеленый
                $('#first_clock_indic').addClass('check');//иконка внутри кнопки появится галочка добавили класс к иконке check
            }
            if(inp === 'second_clock') {
                $('#second_clock').addClass('positive');
                $('#second_clock_indic').addClass('check');
            }

            if(inp === 'crime') {
                $('#crime').addClass('positive');
                $('#crime_indic').addClass('check');
            }

            if(inp === 'crime_close') {
                $('#crime').removeClass('positive');
                $('#crime_indic').removeClass('check');
            }

            if(inp === 'first_clock_2') {
                $('#first_clock_2').addClass('positive');//добавим кнопке класс postive окрасится в зеленый
                $('#first_clock_indic_2').addClass('check');//иконка внутри кнопки появится галочка добавили класс к иконке check
            }
            if(inp === 'second_clock_2') {
                $('#second_clock_2').addClass('positive');
                $('#second_clock_indic_2').addClass('check');
            }
            if(inp === 'open_mansard_door') {
                $('#open_mansard_door').addClass('positive');
                $('#open_mansard_indic').addClass('check');
            }
            if(inp === 'projector') {
                $('#projector').addClass('positive');
                $('#projector_indic').addClass('check');
            }
            if(inp === 'galet_off') {
                $('#open_mansard_door').removeClass('positive');
                $('#open_mansard_indic').removeClass('check');
            }
            if(inp === 'galet_on') {
                $('#open_mansard_door').addClass('positive');
                $('#open_mansard_indic').addClass('check');
            }
            if(inp === 'flag1_on') {
                //используем флажок на всякий
                if(rFlag==0){
                    $('#greenflag').removeClass('white')//удалим класс white
                    $('#greenflag').addClass('green')//добавим класс red
                    rFlag = 1;
                }
            }
            if(inp === 'flag2_on') {
                if(gFlag==0){
                    $('#redflag').removeClass('white')
                    $('#redflag').addClass('red')
                    gFlag = 1;
                }
            }
            if(inp === 'flag3_on') {
                if(bFlag==0){
                    $('#blueflag').removeClass('white')
                    $('#blueflag').addClass('blue')
                    bFlag = 1;
                }
            }
            if(inp === 'flag4_on') {
                if(yFlag==0){
                    $('#yellowflag').removeClass('white')
                    $('#yellowflag').addClass('yellow')
                    yFlag = 1;
                }
            }
            if(inp === 'flag1_off') {
                if(rFlag==1){
                    $('#greenflag').removeClass('green')//удалим класс red 
                    $('#greenflag').addClass('white')//добавим класс white
                    rFlag = 0;
                }
            }
            if(inp === 'flag2_off') {
                if(gFlag==1){
                    $('#redflag').removeClass('red')
                    $('#redflag').addClass('white')
                    gFlag = 0;
                }
            }
            if(inp === 'flag3_off') {
                if(bFlag==1){
                    $('#blueflag').removeClass('blue')
                    $('#blueflag').addClass('white')
                    bFlag = 0;
                }
            }
            if(inp === 'flag4_off') {
                if(yFlag==1){
                    $('#yellowflag').removeClass('yellow')
                    $('#yellowflag').addClass('white')
                    yFlag = 0;
                }
            }

            if (yFlag == 1 && bFlag ==1 && gFlag == 1 && rFlag == 1){
                 $('#open_mansard_stash').addClass('positive');
            }

            if(inp === 'suitcase') { 
                $('#suitcase').addClass('positive');
                $('#suitcase_indic').addClass('check');
            }
            if(inp === 'animals') {
                $('#animals').addClass('positive');
                $('#animals_indic').addClass('check');
            }
            if(inp === 'wolf') {
                $('#wolf').addClass('positive');
                $('#wolf_indic').addClass('check');
            }
            if(inp === 'pedlock') {
                $('#pedlock').addClass('positive');
                $('#pedlock_indic').addClass('check');
            }
            if(inp === 'cat') {
                $('#cat').addClass('positive');
                $('#cat_indic').addClass('check');
            }
            if(inp === 'dog') {
                $('#dog').addClass('positive');
                $('#dog_indic').addClass('check');
            }

            if(inp === 'first_bottle') {
                if(bottle1==0){
                    $('#bottles').progress('increment')
                    bottle1 = 1;
                }
            }
            if(inp === 'second_bottle') {
                if(bottle2==0){
                    $('#bottles').progress('increment')
                    bottle2 = 1;
                }
            }
            if(inp === 'third_bottle') {
                if(bottle3==0){
                    $('#bottles').progress('increment')
                    bottle3 = 1;
                }
            }
            if(inp === 'four_bottle') {
                if(bottle4==0){
                    $('#bottles').progress('increment')
                    bottle4 = 1;
                    $('#open_potions_stash').addClass('positive');
                }
            }
            ////////////////
          
            if(inp === 'mistake_bottle') {   
                $('#bottles').progress({
                    percent: 0
                });
                bottle1=0;
                bottle2=0;
                bottle3=0;
                bottle4=0;
            }
            if(inp === 'dog') {
                $('#dog').addClass('positive');
                $('#dog_indic').addClass('check');
            }
            if(inp === 'owl') {
                $('#owl').addClass('positive');
                $('#owl_indic').addClass('check');
            }
            if(inp === 'owls') {
                $('#owls').addClass('positive');
                $('#owls_indic').addClass('check');
            }
            if(inp === 'train') {
                $('#train').addClass('positive');
                $('#train_indic').addClass('check');
            }
            if(inp === 'mine') {
                $('#mine').addClass('positive');
                $('#mine_indic').addClass('check');
            }
            if(inp === 'troll') {
                $('#troll').addClass('positive');
                $('#troll_indic').addClass('check');
            }
            if(inp === 'open_bank_door') {
                $('#open_bank_door').addClass('positive');
                $('#open_bank_door_indic').addClass('check');
            }
            if(inp === 'safe') {
                $('#safe').addClass('positive');
                $('#safe_indic').addClass('check');
            }
            if(inp === 'safe_close') {
                $('#safe').removeClass('positive');
                $('#safe_indic').removeClass('check');
            }
            if(inp === 'workshop') {
                $('#workshop').addClass('positive');
                $('#workshop_indic').addClass('check');
            }
            if(inp === 'ghost') {
                $('#ghost').addClass('positive');
                $('#ghost_indic').addClass('check');
            }
            if(inp === 'cup') {
                $('#cup').addClass('positive');
                $('#cup_indic').addClass('check');
            }
            if(inp === 'spell') {
                $('#spell').addClass('positive');
                $('#spell_indic').addClass('check');
            }
            if(inp === 'crystals') {
                $('#crystals').addClass('positive');
                $('#crystals_indic').addClass('check');
            }
            if(inp === 'crystals_down') {
                $('#crystals').removeClass('positive');
                $('#crystals_indic').removeClass('check');
            }

            if(inp === 'start_players') {
                $('#players').removeClass('grey');
                $('#players').addClass('green');
                $('#players').addClass('check');
            }
            if(inp === 'stop_players_rest') {
                $('#players').removeClass('green');
                $('#players').addClass('grey');
                $('#players').removeClass('check');
            }

            if(inp === 'stop_players') {
                $('#players').removeClass('green');
                $('#players').addClass('grey');
                $('#players').removeClass('check');
                 goal1bot=0;
                 goal2bot=0;
                 goal3bot=0;
                 goal4bot=0;
                 goal5bot=0;
                 goal6bot=0;
                 goal7bot=0;
                 goal8bot=0;
                 goal9bot=0;
                 goal10bot=0;
                 goal1player=0;
                 goal2player=0;
                 goal3player=0;
                 goal4player=0;
                 goal5player=0;
                 goal6player=0;
                 goal7player=0;
                 goal8player=0;
                 goal9player=0;
                 goal10player=0;
                 $('#on_indic_game').addClass('disabled');
                 $('#player_2_progress').progress({
                    percent: 0
                 });
                 $('#player_2_score').text('0')
                 $('#player_1_progress').progress({
                    percent: 0
                 });
                 $('#player_1_score').text('0')
            }
            //забили гол добавлили под флагом
            if(inp === 'goal_1_bot') {
                if(goal1bot==0){
                    //percent 10 позволяет выставить процент можно в принципе использовать increment но тут так удобнее
                    $('#player_2_progress').progress({
                        percent: 33
                     });
                     //так же вниху под прогресс баром есть текст ставим по количеству
                    $('#player_2_score').text('1')
                    goal1bot = 1;
                }
            }
            if(inp === 'goal_2_bot') {
                if(goal2bot==0){
                    $('#player_2_progress').progress({
                        percent: 66
                     });
                    $('#player_2_score').text('2')
                    goal2bot = 1;
                }
            }
            if(inp === 'win_bot') {
                if(goal10bot==0){
                    $('#player_2_progress').progress({
                    percent: 100
                 });
                    $('#player_2_score').text('3')
                    goal10bot = 1;
                }
            }
            if(inp === 'goal_1_player') {
                if(goal1player==0){
                    $('#player_1_progress').progress({
                        percent: 33
                     });
                    $('#player_1_score').text('1')
                    goal1player = 1;
                }
            }
            if(inp === 'goal_2_player') {
                if(goal2player==0){
                    $('#player_1_progress').progress({
                        percent: 66
                     });
                    $('#player_1_score').text('2')
                    goal2player = 1;
                }
            }
            if(inp === 'win_player') {
                if(goal10player==0){
                    $('#player_1_progress').progress({
                    percent: 100
                 });
                    $('#player_1_score').text('3')
                    $('#basket').addClass('positive');
                    goal10player = 1;
                }
            }

            if(inp === 'first_level') {
                if(crystal1On==1){
                    $('#items_memory').progress({
                    percent: 50
                 });
                    crystal1On = 0;
                }
            }
            if(inp === 'memory_room_end') {
                if(crystal4On==1){
                    $('#items_memory').progress({
                    percent: 100
                 });
                    $('#open_memory_stash').addClass('positive');
                }
                finalAlert=0
            }
            //на последнем сообщении выводим под флагом модальное окно с рейтингом
            if(inp === 'last') {
                if(finalAlert==0){
                    if(bugAlert == 0){
                    swal.fire({
                        title: "Victory!",
                        icon: "success",
                        html: 'Time :' + '&nbsp&nbsp'+timeRate+'<br>'+'Hints :'+'&nbsp&nbsp'+count_help+
                           
                           '<h3>Rating</h3>'+
                           '<div class="ui star huge  rating" data-max-rating="5"></div>'
                   })
                   $('.rating')
                 .rating({
                   initialRating: rate,
                   maxRating: 5
                   
                 });
                 $('.rating').rating('disable')
                    finalAlert = 1;
                    bugAlert = 1;
                    }
                }
               
            }
            if(inp === 'start_error') {
                if(start_error==1 && devices.trim().length > 0){
                    swal.fire({
                        title: "Attention",
                        icon: "warning",
                        html: 'Not connected :' + '&nbsp&nbsp'+devices
                   })
                   start_error = 0;
            }
        }
    
            
            
// выбор языка
            if(inp === 'russian') {
                $('#language').text('Russian');
            }
            if(inp === 'english') {
                $('#language').text('English');
            }
            if(inp === 'arabian') {
                $('#language').text('Arabian');
            }
            
                
            
            if(inp === 'HIGH') {
                $('#battary_indic').removeClass('half');
                $('#battary_indic').removeClass('quarter');
                $('#battary_indic').removeClass('red');
                $('#battary_indic').removeClass('yellow');
                $('#battary_indic').transition('remove looping')
                $('#battary_indic').addClass('full');
                $('#battary_indic').addClass('green');
                
            }
            if(inp === 'NORMAL') {
                $('#battary_indic').removeClass('full');
                $('#battary_indic').transition('remove looping')
                $('#battary_indic').removeClass('quarter');
                $('#battary_indic').removeClass('red');
                $('#battary_indic').removeClass('green');
                $('#battary_indic').addClass('half');
                $('#battary_indic').addClass('yellow');
            }
            if(inp === 'LOW') {
                $('#battary_indic').removeClass('full');
                $('#battary_indic').removeClass('half');
                $('#battary_indic').removeClass('yellow');
                $('#battary_indic').removeClass('green');
                $('#battary_indic').addClass('quarter');
                $('#battary_indic').addClass('red');
                $('#battary_indic')
    .transition('set looping')
    .transition('pulse', '700ms');
            }
            
            ////////////////////////////////
            if(inp === 'rest') {
                //если пришло с сервера rest все скидываем обновляем приводим к виду по умолчанию
                $('#Restart').css('border','red 2px solid');
                $('#estart').addClass('loading'); 
                //$('#Start').css('border','white 2px solid');
                $('#Start').css('border','grey 2px solid');
                $('#start_icon').removeClass('green');
                $('#start_icon').addClass('grey');
                $('#Start').addClass('disabled');
                $('#Pause').css('border','white 2px solid');
                $('#Ready').css('border','white 2px solid');
                $('#ready_icon').removeClass('gray');
                $('#ready_icon').addClass('orange');
                $('#Ready').removeClass('disabled');
                
                if(restflag==0){
                $('.button').removeClass('positive');
                $('.button').removeClass('disabled');
                $('.icon').removeClass('check');
				
				// Сброс иконок и прогресс-баров ---
                $('#helmet_icon').removeClass('green');
                $('#broom_icon').removeClass('green');

                $('#safe_progress').progress({ percent: 0 });
                $('#troll_progress').progress({ percent: 0 });
                $('#ghost_progress').progress({ percent: 0 });
                $('#owls_progress').progress({ percent: 0 });
				$('#spell_progress').progress({ percent: 0 });

                $('#first_door').removeClass('open');
                $('#first_door').removeClass('green');

                $('#door_puzzle').removeClass('open');
                $('#door_puzzle').removeClass('green');

                $('#bottles').progress({
                    percent: 0
                });
                $('#player_1_progress').progress({
                    percent: 0
                });
                $('#player_2_progress').progress({
                    percent: 0
                });
                $('#items_memory').progress({
                    percent: 0
                });
                
                
                $('#players').removeClass('grey');
                $('#players').removeClass('disabled');
                $('#players').addClass('green');

                $('#yellowflag').removeClass('yellow')
                $('#yellowflag').addClass('white')
                $('#redflag').removeClass('red')
                $('#redflag').addClass('white')
                $('#greenflag').removeClass('green')
                $('#greenflag').addClass('white')
                $('#blueflag').removeClass('blue')
                $('#blueflag').addClass('white')
                $('#player_1_score').text('0')
                $('#player_2_score').text('0')
                disBut=0;
                restflag =1;
            }
     
            }
            if(inp === 'pause_game') {
                $('#Restart').css('border','white 2px solid') ;
                $('#estart').removeClass('loading'); 
                $('#Start').css('border','white 2px solid') ;
                $('#Pause').css('border','yellow 2px solid') ;
                $('#Ready').css('border','white 2px solid') ;
            }
            if(inp === 'start_game') {
                restflag = 0;
                $('#Ready').css('border','grey 2px solid');
                $('#ready_icon').removeClass('orange');
                $('#ready_icon').addClass('gray');
                $('#Ready').addClass('disabled');

                $('#Start').css('border','green 2px solid');
                //если пришло с сервера start_game запускаем нашуигру блокируем все окна кроме первого уровня 
                $('#Restart').css('border','white 2px solid') ;
                $('#estart').removeClass('loading'); 
                $('#Start').css('border','green 2px solid');
                $('#Pause').css('border','white 2px solid');
                if(disBut==0){
                $('.button').removeClass('positive');
                $('.icon').removeClass('check');
                $('#bottles').progress({
                    percent: 0
                });
                $('#player_1_progress').progress({
                    percent: 0
                });
                $('#player_2_progress').progress({
                    percent: 0
                });
                $('#items_memory').progress({
                    percent: 0
                });
                $('#yellowflag').removeClass('yellow')
                $('#yellowflag').addClass('white')
                $('#redflag').removeClass('red')
                $('#redflag').addClass('white')
                $('#greenflag').removeClass('green')
                $('#greenflag').addClass('white')
                $('#blueflag').removeClass('blue')
                $('#blueflag').addClass('white')
                $('#player_1_score').text('0')
                $('#player_2_score').text('0')
                $('#projector').addClass('disabled');
                $('#first_clock').addClass('disabled');
                $('#second_clock').addClass('disabled');
                $('#first_clock_2').addClass('disabled');
                $('#second_clock_2').addClass('disabled');
                $('#open_mansard_door').addClass('disabled');
                $('#suitcase').addClass('disabled');
                $('#animals').addClass('disabled');
                $('#crime').addClass('disabled');
                $('#wolf').addClass('disabled');
                $('#open_mansard_stash').addClass('disabled');
                $('#pedlock').addClass('disabled');
                $('#dog').addClass('disabled');
                $('#cat').addClass('disabled');
                $('#open_potions_stash').addClass('disabled');
                $('#owl').addClass('disabled');
                $('#owls').addClass('disabled');
                $('#train').addClass('disabled');
                $('#mine').addClass('disabled');
                $('#troll').addClass('disabled');
                $('#open_bank_door').addClass('disabled');
                $('#safe').addClass('disabled');
                $('#workshop').addClass('disabled');
                $('#ghost').addClass('disabled');
                $('#cup').addClass('disabled');
                $('#spell').addClass('disabled');
                $('#crystals').addClass('disabled');
                $('#open_memory_stash').addClass('disabled');
                $('#open_memory_stash').addClass('disabled');
                $('#players').removeClass('green');
                $('#players').addClass('grey');
                $('#basket').addClass('disabled');
                disBut=1;
                }
            }
            if(inp === 'ready') {
                $('#Restart').css('border','white 2px solid');
                $('#estart').removeClass('loading');
                $('#Start').css('border','white 2px solid');
                $('#Pause').css('border','white 2px solid');
                $('#Ready').css('border','orange 2px solid');
                $('#start_icon').removeClass('grey');
                $('#start_icon').addClass('green');
                $('#Start').removeClass('disabled');
                restflag = 0;
            }
			// --- НАЧАЛО ИЗМЕНЕНИЯ: Блокировка и разблокировка кнопки "Ready" ---
            if(inp === 'ready_processing') {
                $('#Ready').addClass('disabled loading'); // Блокируем кнопку и добавляем анимацию загрузки
            }
            if(inp === 'ready_finished') {
                $('#Ready').removeClass('disabled loading'); // Снимаем блокировку и анимацию
            }
            // --- КОНЕЦ ИЗМЕНЕНИЯ ---
        }
    });



    ///////////////////////remote
    //нажали пропустить тумблер
    $('#first_clock').click(function(){
        socket.emit('Remote','first_clock')//отправляем данные на сервер в декоратор remote строку tumbler
    });

    $('#second_clock').click(function(){
        socket.emit('Remote','second_clock')
    });

    $('#first_clock_2').click(function(){
        socket.emit('Remote','first_clock_2')//отправляем данные на сервер в декоратор remote строку tumbler
    });

    $('#second_clock_2').click(function(){
        socket.emit('Remote','second_clock_2')
    });

    $('#crime').click(function(){
        socket.emit('Remote','crime')
    });
    $('#open_mansard_door').click(function(){
        socket.emit('Remote','open_mansard_door')
    });

    $('#suitcase').click(function(){
        socket.emit('Remote','suitcase')
    });
    $('#animals').click(function(){
        socket.emit('Remote','animals')
    });

     $('#power_off').click(function(){
        socket.emit('Remote','off')
    });

    $('#wolf').click(function(){
        socket.emit('Remote','wolf')
    });
    $('#projector').click(function(){
        socket.emit('Remote','projector')
    });
    $('#open_mansard_stash').click(function(){
        socket.emit('Remote','open_mansard_stash')
    });
    $('#pedlock').click(function(){
        socket.emit('Remote','pedlock')
    });
    $('#dog').click(function(){
        socket.emit('Remote','dog')
    });
    $('#cat').click(function(){
        socket.emit('Remote','cat')
    });
    $('#open_potions_stash').click(function(){
        socket.emit('Remote','open_potions_stash')
    });

    $('#owl').click(function(){
        socket.emit('Remote','owl')
    });
    $('#owls').click(function(){
        socket.emit('Remote','owls')
    });
    $('#train').click(function(){
        socket.emit('Remote','train')
    });
    $('#mine').click(function(){
        socket.emit('Remote','mine')
    });
    $('#troll').click(function(){
        socket.emit('Remote','troll')
    });
    $('#open_bank_door').click(function(){
        socket.emit('Remote','open_bank_door')
    });
    $('#safe').click(function(){
        socket.emit('Remote','safe')
    });
    $('#workshop').click(function(){
        socket.emit('Remote','workshop')
    });

    $('#ghost').click(function(){
        socket.emit('Remote','ghost')
    });
    $('#cup').click(function(){
        socket.emit('Remote','cup')
    });
    $('#spell').click(function(){
        socket.emit('Remote','spell')
    });
    $('#crystals').click(function(){
        socket.emit('Remote','crystals')
    });
    $('#open_memory_stash').click(function(){
        socket.emit('Remote','open_memory_stash')
    });
    $('#basket').click(function(){
        socket.emit('Remote','basket')
    });

    //если нажимаем на выбор языка отправляем на сервер данные о выбранном языке
    $('#russian').click(function(){
        socket.emit('Remote','russian')
    }); 
    $('#english').click(function(){
        socket.emit('Remote','english')
    });   
    $('#arabian').click(function(){
        socket.emit('Remote','arabian')
    });  

    $('#Castle1').click(function () {
        socket.emit('WLAN', 'ssid=Castle1')
    });
    $('#Castle2').click(function () {
        socket.emit('WLAN', 'ssid=Castle2')
    });
    $('#Castle3').click(function () {
        socket.emit('WLAN', 'ssid=Castle3')
    });
    $('#Castle4').click(function () {
        socket.emit('WLAN', 'ssid=Castle4')
    });
    $('#Castle5').click(function () {
        socket.emit('WLAN', 'ssid=Castle5')
    });
    $('#Castle6').click(function () {
        socket.emit('WLAN', 'ssid=Castle6')
    });
    $('#Castle7').click(function () {
        socket.emit('WLAN', 'ssid=Castle7')
    });
    $('#Castle8').click(function () {
        socket.emit('WLAN', 'ssid=Castle8')
    });
    $('#Castle9').click(function () {
        socket.emit('WLAN', 'ssid=Castle9')
    });
    $('#Castle10').click(function () {
        socket.emit('WLAN', 'ssid=Castle10')
    });
    
			//управляем громкостью
			// Логика переписана: UI обновляется только при получении ответа от сервера.

			// --- Effect Sound ---
			buttonUpEffect.addEventListener(start, function() {
				timerIntervalUpEffect = setInterval(function(){
					var newValue = lev12 + 1;
					if(newValue <= 100){
						socket.emit('Effects', newValue);
					} else {
						clearInterval(timerIntervalUpEffect); // Останавливаем, если достигли максимума
					}
			  }, 100);
			});
			buttonUpEffect.addEventListener(leave, function() { clearInterval(timerIntervalUpEffect); });
			buttonUpEffect.addEventListener(stop, function() { clearInterval(timerIntervalUpEffect); });
			$('#effectUp').click(function(){
				var newValue = lev12 + 1;
				if(newValue <= 100){ socket.emit('Effects', newValue); }
			});
			
			buttonDownEffect.addEventListener(start, function() {
				timerIntervalDownEffect = setInterval(function(){
					var newValue = lev12 - 1;
					if(newValue >= 0){
						socket.emit('Effects', newValue);
					} else {
						clearInterval(timerIntervalDownEffect); // Останавливаем, если достигли минимума
					}
				}, 100);
			});
			buttonDownEffect.addEventListener(leave, function() { clearInterval(timerIntervalDownEffect); });
			buttonDownEffect.addEventListener(stop, function() { clearInterval(timerIntervalDownEffect); });
			$('#effectDown').click(function(){
				var newValue = lev12 - 1;
				if(newValue >= 0){ socket.emit('Effects', newValue); }
			});

			// --- Phone Sound ---
			buttonUpPhone.addEventListener(start, function() {
				timerIntervalUpPhone = setInterval(function(){
					var newValue = lev11 + 1;
					if(newValue <= 100){
						socket.emit('Phone', newValue);
					} else {
						clearInterval(timerIntervalUpPhone);
					}
			  }, 100);
			});
			buttonUpPhone.addEventListener(leave, function() { clearInterval(timerIntervalUpPhone); });
			buttonUpPhone.addEventListener(stop, function() { clearInterval(timerIntervalUpPhone); });
			$('#phoneUp').click(function(){
				var newValue = lev11 + 1;
				if(newValue <= 100){ socket.emit('Phone', newValue); }
			});

			buttonDownPhone.addEventListener(start, function() {
				timerIntervalDownPhone = setInterval(function(){
					var newValue = lev11 - 1;
					if(newValue >= 0){
						socket.emit('Phone', newValue);
					} else {
						clearInterval(timerIntervalDownPhone);
					}
				}, 100);
			});
			buttonDownPhone.addEventListener(leave, function() { clearInterval(timerIntervalDownPhone); });
			buttonDownPhone.addEventListener(stop, function() { clearInterval(timerIntervalDownPhone); });
			$('#phoneDown').click(function(){
				var newValue = lev11 - 1;
				if(newValue >= 0){ socket.emit('Phone', newValue); }
			});

			// --- Voice Sound ---
			buttonUpVoice.addEventListener(start, function() {
				timerIntervalUpVoice = setInterval(function(){
					var newValue = lev13 + 1;
					if(newValue <= 100){
						socket.emit('Voice', newValue);
					} else {
						clearInterval(timerIntervalUpVoice);
					}
			  }, 100);
			});
			buttonUpVoice.addEventListener(leave, function() { clearInterval(timerIntervalUpVoice); });
			buttonUpVoice.addEventListener(stop, function() { clearInterval(timerIntervalUpVoice); });
			$('#voiceUp').click(function(){
				var newValue = lev13 + 1;
				if(newValue <= 100){ socket.emit('Voice', newValue); }
			});

			buttonDownVoice.addEventListener(start, function() {
				timerIntervalDownVoice = setInterval(function(){
					var newValue = lev13 - 1;
					if(newValue >= 0){
						socket.emit('Voice', newValue);
					} else {
						clearInterval(timerIntervalDownVoice);
					}
				}, 100);
			});
			buttonDownVoice.addEventListener(leave, function() { clearInterval(timerIntervalDownVoice); });
			buttonDownVoice.addEventListener(stop, function() { clearInterval(timerIntervalDownVoice); });
			$('#voiceDown').click(function(){
				var newValue = lev13 - 1;
				if(newValue >= 0){ socket.emit('Voice', newValue); }
			});


			// --- Wolf Sound ---
			buttonUpWolf.addEventListener(start, function() {
				timerIntervalUpWolf = setInterval(function(){
					var newValue = lev14 + 1;
					if(newValue <= 30){
						socket.emit('WolfUp', newValue);
					} else {
						clearInterval(timerIntervalUpWolf);
					}
			  }, 100);
			});
			buttonUpWolf.addEventListener(leave, function() { clearInterval(timerIntervalUpWolf); });
			buttonUpWolf.addEventListener(stop, function() { clearInterval(timerIntervalUpWolf); });
			$('#wolfUp').click(function(){
				var newValue = lev14 + 1;
				if(newValue <= 30){ socket.emit('WolfUp', newValue); }
			});
			
			buttonDownWolf.addEventListener(start, function() {
				timerIntervalDownWolf = setInterval(function(){
					var newValue = lev14 - 1;
					if(newValue >= 0){
						socket.emit('WolfDown', newValue);
					} else {
						clearInterval(timerIntervalDownWolf);
					}
				}, 100);
			});
			buttonDownWolf.addEventListener(leave, function() { clearInterval(timerIntervalDownWolf); });
			buttonDownWolf.addEventListener(stop, function() { clearInterval(timerIntervalDownWolf); });
			$('#wolfDown').click(function(){
				var newValue = lev14 - 1;
				if(newValue >= 0){ socket.emit('WolfDown', newValue); }
			});

			// --- Platform Sound ---
			buttonUpPlatform.addEventListener(start, function() {
				timerIntervalUpPlatform = setInterval(function(){
					var newValue = lev15 + 1;
					if(newValue <= 30){
						socket.emit('PlatformUp', newValue);
					} else {
						clearInterval(timerIntervalUpPlatform);
					}
			  }, 100);
			});
			buttonUpPlatform.addEventListener(leave, function() { clearInterval(timerIntervalUpPlatform); });
			buttonUpPlatform.addEventListener(stop, function() { clearInterval(timerIntervalUpPlatform); });
			$('#platformUp').click(function(){
				var newValue = lev15 + 1;
				if(newValue <= 30){ socket.emit('PlatformUp', newValue); }
			});

			buttonDownPlatform.addEventListener(start, function() {
				timerIntervalDownPlatform = setInterval(function(){
					var newValue = lev15 - 1;
					if(newValue >= 0){
						socket.emit('PlatformDown', newValue);
					} else {
						clearInterval(timerIntervalDownPlatform);
					}
				}, 100);
			});
			buttonDownPlatform.addEventListener(leave, function() { clearInterval(timerIntervalDownPlatform); });
			buttonDownPlatform.addEventListener(stop, function() { clearInterval(timerIntervalDownPlatform); });
			$('#platformDown').click(function(){
				var newValue = lev15 - 1;
				if(newValue >= 0){ socket.emit('PlatformDown', newValue); }
			});

			// --- Suitcases Sound ---
			buttonUpSuitcases.addEventListener(start, function() {
				timerIntervalUpSuitcases = setInterval(function(){
					var newValue = lev16 + 1;
					if(newValue <= 30){
						socket.emit('SuitcasesUp', newValue);
					} else {
						clearInterval(timerIntervalUpSuitcases);
					}
			  }, 100);
			});
			buttonUpSuitcases.addEventListener(leave, function() { clearInterval(timerIntervalUpSuitcases); });
			buttonUpSuitcases.addEventListener(stop, function() { clearInterval(timerIntervalUpSuitcases); });
			$('#suitcasesUp').click(function(){
				var newValue = lev16 + 1;
				if(newValue <= 30){ socket.emit('SuitcasesUp', newValue); }
			});

			buttonDownSuitcases.addEventListener(start, function() {
				timerIntervalDownSuitcases = setInterval(function(){
					var newValue = lev16 - 1;
					if(newValue >= 0){
						socket.emit('SuitcasesDown', newValue);
					} else {
						clearInterval(timerIntervalDownSuitcases);
					}
				}, 100);
			});
			buttonDownSuitcases.addEventListener(leave, function() { clearInterval(timerIntervalDownSuitcases); });
			buttonDownSuitcases.addEventListener(stop, function() { clearInterval(timerIntervalDownSuitcases); });
			$('#suitcasesDown').click(function(){
				var newValue = lev16 - 1;
				if(newValue >= 0){ socket.emit('SuitcasesDown', newValue); }
			});

			// --- Safe Sound ---
			buttonUpSafe.addEventListener(start, function() {
				timerIntervalUpSafe = setInterval(function(){
					var newValue = lev17 + 1;
					if(newValue <= 30){
						socket.emit('SafeUp', newValue);
					} else {
						clearInterval(timerIntervalUpSafe);
					}
			  }, 100);
			});
			buttonUpSafe.addEventListener(leave, function() { clearInterval(timerIntervalUpSafe); });
			buttonUpSafe.addEventListener(stop, function() { clearInterval(timerIntervalUpSafe); });
			$('#safeUp').click(function(){
				var newValue = lev17 + 1;
				if(newValue <= 30){ socket.emit('SafeUp', newValue); }
			});

			buttonDownSafe.addEventListener(start, function() {
				timerIntervalDownSafe = setInterval(function(){
					var newValue = lev17 - 1;
					if(newValue >= 0){
						socket.emit('SafeDown', newValue);
					} else {
						clearInterval(timerIntervalDownSafe);
					}
				}, 100);
			});
			buttonDownSafe.addEventListener(leave, function() { clearInterval(timerIntervalDownSafe); });
			buttonDownSafe.addEventListener(stop, function() { clearInterval(timerIntervalDownSafe); });
			
			$('#safeDown').click(function(){
				// (последний клик) ---
				var newValue = lev17 - 1;
				if(newValue >= 0){ socket.emit('SafeDown', newValue); }
			});
			buttonDownSafe.addEventListener(stop, function() {
				clearInterval(timerIntervalDownSafe);
			});


      socket.on('timer', function(e) {
//обновляем текст с таймером
if (typeof(e) === 'string') {
$('#output').text(e);   
}
 });
});
