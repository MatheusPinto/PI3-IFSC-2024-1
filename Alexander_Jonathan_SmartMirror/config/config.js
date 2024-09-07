let config = {
	address: "localhost",	// Address to listen on, can be:
							// - "localhost", "127.0.0.1", "::1" to listen on loopback interface
							// - another specific IPv4/6 to listen on a specific interface
							// - "0.0.0.0", "::" to listen on any interface
							// Default, when address config is left out or empty, is "localhost"
	port: 8080,
	basePath: "/",
									// you must set the sub path here. basePath must end with a /
	ipWhitelist: ["127.0.0.1", "::ffff:127.0.0.1", "::1"],	// Set [] to allow all IP addresses
									// or add a specific IPv4 of 192.168.1.5 :
									// ["127.0.0.1", "::ffff:127.0.0.1", "::1", "::ffff:192.168.1.5"],
									// or IPv4 range of 192.168.3.0 --> 192.168.3.15 use CIDR format :
									// ["127.0.0.1", "::ffff:127.0.0.1", "::1", "::ffff:192.168.3.0/28"],

	useHttps: false,				// Support HTTPS or not, default "false" will use HTTP
	httpsPrivateKey: "",			// HTTPS private key path, only require when useHttps is true
	httpsCertificate: "",			// HTTPS Certificate path, only require when useHttps is true

	language: "pt",					// to syncronize my language
	locale: "pt",
	logLevel: ["INFO", "LOG", "WARN", "ERROR"], // Add "DEBUG" for even more logging
	timeFormat: 24,
	units: "metric",

	modules: [
		//// ****************************************** keybindings module ********************************************
		//{	
			//module: 'KeyBindings',
			//config: {
				//evdev: { enabled: false },
				//enableKeyboard: true,
			//}
		//},
		// ******************************************** carousel module ********************************************
		//{
		  //module: "Carousel",
		  //position: "bottom_bar", 					// Required only for navigation controls
		  //config: {
			//transitionInterval: 0, 					// passing 0 with mode:"slides" will disable the timer for manual navigation
			//showPageIndicators: true, 				// a boolean true or false to show or hide the page indication 'bubbles' at the bottom of the screen
			//showPageControls: true, 				// a boolean true or false to show or hide the next and previous page buttons
			//ignoreModules: ["clock", "alert"], 		// a list of module names whom should not be considered as part of the carousel
			//mode: "slides", 						// groups of modules can be assigned to be displayed at the same time
			//slides: {
			  //main: [								// home carousel page
				//"calendar",
				//"compliments",
				//"weather",
				//"newsfeed"
			  //], 
			  //"Slide 2": [							// second carousel page
				//"compliments",
				//"calendar"
			  //],
			  //"Slide 3": [							// third carousel page
				//"weather"
			  //]
			//},
			//keyBindings: {							// enable keuboard switch pages (not working :/)
			  //enabled: true,
			  //map: {
				//NextSlide: "ArrowRight",
				//PrevSlide: "ArrowLeft",
				//Pause: "ArrowDown",
				//Slide0: "Home"
			  //},
			  //mode: "DEFAULT"
			//}
		  //}
		//},
		// ******************************************** alert module ********************************************
		{
			module: "alert",
		},
		// ************************************* update notification module ********************************************
		//{
			//module: "updatenotification",
			//position: "top_bar"
		//},
		// ************************************* learning module ********************************************
		{
			module: "MagicLearning",
			position: "top_bar"
		},
		// ****************************************** gesture module ********************************************		
		{
		module: "GroveGestures",
		position: "top_bar",
		config: {
			autoStart: true, 			//When Mirror starts, recognition will start.
			verbose:true, 				// If set as `true`, useful messages will be logged.
			recognitionTimeout: 1000, 	//Gesture sequence will be ended after this time from last recognized gesture.
			
			pythonPath: "/usr/bin/python",
			
			// set idle time to hide all modules
			idleTimer: 10000, //*60*30, // `0` for disable, After this time from last gesture, onIdle will be executed.
			onIdle: { // See command section
				moduleExec: {
					module: [],
					exec: (module, gestures) => {
						module.hide(1000)
					}
				}
			},
			// if s gesture is recognized, all modules shows again
			onDetected: {
			notificationExec: {
				notification: "GESTURE_DETECTED",
				},
				// You can make Mirror to wake up the modules which were hidden by onIdle with any gestures.
				moduleExec: {
					module: [],
					exec: (module) => {
						module.show()
					}
				}
			},
			
			// defaultCommandSet: "default",
			commandSet: {
					"default": {
						"DOWN": {
							moduleExec: {
							module: [],
							exec: (module, gestures) => {
								module.hide(1000)
								}
							}
						},
						"UP": {
							moduleExec: {
								module: [],
								exec: (module, gestures) => {
								module.show()
								}
							}
						},
						"LEFT": {
							notificationExec: {
								notification: "PAGE_DECREMENT",
								payload: null,
							}
						},
						"RIGHT": {
								notificationExec: {
									notification: "PAGE_INCREMENT",
									payload: null,
							}
						},
					},
				},
			}
		},
		// *************************************** pages module ********************************************
		{
		module: "pages",
		config: {
			rotationTime: 1800000,
			modules: [
				[
					"calendar",				// top_left
					"compliments",			// lower_third
					"weather",				// top_right
					"newsfeed"				// bottom_bar
				],
				[
					"Formula1",			// top_left
					"CalendarExt3Agenda", // top_right
					"compliments",			// lower_third
				],
				[
					"weather",				// top_right
					"Spotify",
					"systemtemperature"
				]
			],
			fixed: [
					"clock",				// top_bar
					"network-signal",	// bottom_right
					"page-indicator", 	// bottom_bar
					
					"MagicLearning"		// test
			]
			} 	
		},
		// ************************************** pages indicator module ********************************************
		{
		module: 'page-indicator',
		position: 'bottom_bar',
		config: {
			pages: 3,
			}
		},
		// *************************************** gesture simple module ********************************************
		//{
			//module: "GroveGestures",
			//position: "top_bar",
			//config: {}
		//},
		// ******************************************** clock module ********************************************
		{
			module: "clock",
			position: "top_bar"
		},
		// ******************************************** calendar module ********************************************
		{
			module: "calendar",
			header: "Proximos Eventos",
			position: "top_left",
			config: {
				calendars: [
					{
						fetchInterval: 7 * 24 * 60 * 60 * 1000,
						symbol: "calendar-check",	// hollidays calendar
						url: "https://www.supercalendario.com.br/ics/2024"
					},
					{
						symbol: "book", 			// my personal calendar
						url: "webcal://p166-caldav.icloud.com/published/2/MTkxNTE5MjExODE5MTUxOUlBAJ8rEfjrybsOf3ZVL_MSwgevaOtpeTSkITb9ifIAnvFXwJz1Jj47tsN6Ccx4Hn5TrQfOF1Ya9BZkdfHUnAQ"
					},
					{
						symbol: "bookmark", 		// my personal calendar
						url: "webcal://p166-caldav.icloud.com/published/2/MTkxNTE5MjExODE5MTUxOUlBAJ8rEfjrybsOf3ZVL_NEy_DPBqtdqM4y7Cq2ja_sw28rjufetq-1gOqDnmFon81jc08KfmHnItrx_ZrNgUo"
					},
					{
						symbol: "car", 				// formula 1
						url: "webcal://racingnews365.com/ics/download/calendar-formula-2024.ics?all"
					}
				]
			}
		},
		// ****************************************** network signal module ********************************************
		{
		module: "network-signal",
		position: "bottom_right",
		config: {
			animationSpeed: 100,
			showMessage: false
			}
		},
		// ******************************************* compliments module ********************************************
		{
			module: "compliments",
			position: "lower_third"
		},
		// ******************************************** weather module ********************************************
		{
			module: "weather",
			position: "top_right",
			config: {
				weatherProvider: "openweathermap",
				type: "current",
				location: "Florianópolis",
				locationID: "3463237", //ID from http://bulk.openweathermap.org/sample/city.list.json.gz;
				apiKey: "API_KEY"
			}
		},
		// ******************************************* forecast module ********************************************
		{
			module: "weather",
			position: "top_right",
			header: "Previsão do Tempo",
			config: {
				weatherProvider: "openweathermap",
				type: "forecast",
				location: "Florianópolis",
				locationID: "3463237", //ID from http://bulk.openweathermap.org/sample/city.list.json.gz;
				apiKey: "API_KEY"
			}
		},
		// ******************************************* newsfeed module ********************************************
		{
			module: "newsfeed",
			position: "bottom_bar",
			config: {
				feeds: [
					{
						title: "Notícias Minuto",
						url: "https://www.noticiasaominuto.com.br/rss/tech"
					},
					{
						title: "Tec Mundo",
						url: "https://rss.tecmundo.com.br/feed"
					},
					{
						title: "Notícias do IFSC",
						url: "https://repositorio.ifsc.edu.br/feed/rss_2.0/123456789/12"
					}
				],
				showSourceTitle: true,
				showPublishDate: true,
				broadcastNewsFeeds: true,
				broadcastNewsUpdates: true
			}
		},
		// ****************************************** formula 1 module ********************************************
		{
			module: "Formula1",
			position: "top_left",
			header: "Formula 1",
			config: {
					showStanding: "BOTH",
					maxRowsDriver: 3,
					maxRowsConstructor: 3,
					showSchedule: true,
					// showNextRace: true
			}
		},
		// ******************************************* agenda module ********************************************
		//{
			//module: "CalendarExt3Agenda",
			//position: "top_right",
		//},
		{
			module: "CalendarExt3Agenda",
			position: "top_right",
			title: "Agenda",
			config: {
				instanceId: "basicCalendar",
				locale: 'pt-BR',
				firstDayOfWeek: 1,
				startDayIndex: -1,
				endDayIndex: 10,
				calendarSet: [],
			}
		},
		// ******************************************* spotify module ********************************************
		{
			module: "Spotify",
			position: "bottom_left",
			config: {
					debug: false,
				}
		},
		// ******************************************* system temperature module ********************************************
		{
			module: 'systemtemperature',
			position: 'top_center',
			classes: 'small dimmed',
			config: {
				prependString: "Temperatura do Sistema"
			}
		}
	]
};

/*************** DO NOT EDIT THE LINE BELOW ***************/
if (typeof module !== "undefined") { module.exports = config; }
