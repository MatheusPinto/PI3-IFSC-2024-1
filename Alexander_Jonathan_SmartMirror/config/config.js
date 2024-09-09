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

	// ******************** TEST YT WEB VIEW
	electronOptions: {
		webPreferences: {
			webviewTag: true,
			contextIsolation: false,
			enableRemoteModule: true
		},
	},


	modules: [

		// **************************************************************************************** BEGIN

		// **************************************************************************************** INACTIVE: key bindings module

		//{	
			//module: 'KeyBindings',
			//config: {
				//evdev: { enabled: false },
				//enableKeyboard: true,
			//}
		//},

		// **************************************************************************************** INACTIVE: carousel module

		//{
		  //module: "Carousel",
		  //position: "bottom_bar", 					// Required only for navigation controls
		  //config: {
			//transitionInterval: 0, 					
			//showPageIndicators: true, 				
			//showPageControls: true, 				
			//ignoreModules: ["clock", "alert"], 		
			//mode: "slides", 						
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

		// **************************************************************************************** INACTIVE: alert module

		{
			module: "alert",
		},

		// **************************************************************************************** INACTIVE: update notification module

		//{
			//module: "updatenotification",
			//position: "top_bar"
		//},

		// **************************************************************************************** FOURTH PAGE TESTING BROKEN: magic learning module

		{
			module: "MagicLearning",
			position: "bottom_bar"
		},

		// **************************************************************************************** FIXED: grove gestures module
		
		{
		module: "GroveGestures",
		position: "top_bar",
		config: {
			autoStart: true, 			// When Mirror starts, recognition will start.
			verbose:true, 				
			recognitionTimeout: 1000, 	// Gesture sequence will be ended after this time from last recognized gesture.
			
			pythonPath: "/usr/bin/python",
			
			// set idle time to hide all modules
			idleTimer: 100000, //*60*30, // `0` for disable, After this time from last gesture, onIdle will be executed.
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
				// wake up the modules which were hidden by onIdle with any gestures.
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

		// **************************************************************************************** FIXED: pages module

		{
		module: "pages",
		config: {
			rotationTime: 1800000,
			modules: [
				[
					"CalendarExt3Agenda", 		// left
					"compliments",				// bottom_bar
				],
				[
					"Formula1",					// left
					"MplayerRadio",				// bottom_left
					"newsfeed",					// bottom_bar
				],
				[
					"SmartWebDisplay",			// left
					"systemtemperature",		// bottom_left
					"network-signal",			// bottom_left
				],
				[
					"Spotify",					// left
					// "MagicLearning",			// bottom_bar
					"YouTubeWebView",			// bottom_left
					"BusTimes",					// top_right
				]
			],
			fixed:  [
					"clock",					// top_left
					"page-indicator", 			// bottom_bar
				]
			} 	
		},

		// **************************************************************************************** FIXED: pages indicator module

		{
			module: 'page-indicator',
			position: 'bottom_bar',
			config: {
				pages: 3,
				}
		},

		// **************************************************************************************** INACTIVE: gesture simple module

		//{
			//module: "GroveGestures",
			//position: "top_bar",
			//config: {}
		//},

		// **************************************************************************************** FIXED: clock module

		{
			module: "clock",
			position: "top_left"
		},

		// **************************************************************************************** INACTIVE: simple calendar module

		{
			module: "calendar",
			header: "Proximos Eventos",
			position: "left",
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

		// **************************************************************************************** FIRST PAGE: compliments module

		{
			module: "compliments",
			position: "bottom_bar",
			config: {
				classes: "thin bright"
			}
		},

		// **************************************************************************************** FIRST PAGE: weather module

		{
			module: "weather",
			position: "bottom_left",
			config: {
				weatherProvider: "openweathermap",
				type: "current",
				location: "Florianópolis",
				locationID: "3463237", //ID from http://bulk.openweathermap.org/sample/city.list.json.gz;
				apiKey: "3892217fdda3dac276b2a4919e818839"
			}
		},

		// **************************************************************************************** FIRST PAGE: forecast module

		{
			module: "weather",
			position: "bottom_left",
			header: "Previsão do Tempo",
			config: {
				weatherProvider: "openweathermap",
				type: "forecast",
				location: "Florianópolis",
				locationID: "3463237", //ID from http://bulk.openweathermap.org/sample/city.list.json.gz;
				apiKey: "3892217fdda3dac276b2a4919e818839"
			}
		},

		// **************************************************************************************** SECOND PAGE: newsfeed module

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

		// **************************************************************************************** SECOND PAGE: f1 module

		{
			module: "Formula1",
			position: "left",
			header: "Formula 1",
			config: {
					showStanding: "BOTH",
					maxRowsDriver: 3,
					maxRowsConstructor: 3,
					showSchedule: true,
					// showNextRace: true
			}
		},

		// **************************************************************************************** FIRST PAGE: agenda module

		//{
			//module: "CalendarExt3Agenda",
			//position: "top_right",
		//},

		{
			module: "CalendarExt3Agenda",
			position: "left",
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

		// **************************************************************************************** FOURTH PAGE: spotify module

		{
			module: "Spotify",
			position: "left",
			config: {
					debug: false,
				}
		},

		// **************************************************************************************** THIRD PAGE: system temperature module

		{
			module: 'systemtemperature',
			position: 'bottom_left',
			classes: 'small dimmed',
			config: {
				prependString: "Temperatura do Sistema"
			}
		},

		// **************************************************************************************** THIRD PAGE: network signal module

		{
			module: "network-signal",
			position: "bottom_left",
			config: {
				animationSpeed: 100,
				showMessage: false
				}
		},

		// **************************************************************************************** THIRD PAGE: cams module

		{
			module: 'SmartWebDisplay', // first cam
			position: 'left',
			config: {
				logDebug: false,
				height:"100%",
				width:"100%",
				updateInterval: 0,
				NextURLInterval: 0,
				displayLastUpdate: false,
				displayLastUpdateFormat: 'ddd - HH:mm:ss',
				url: ["https://video.nest.com/embedded/live/rsStd5iVLE?autoplay=1"],
				scrolling: "no",
				shutoffDelay: 10000 
			}
		},
		{
			    module: 'SmartWebDisplay', // second cam
			    position: 'left', // same pos
			    config: {
				logDebug: false,
				height: "50%", 
				width: "100%",
				updateInterval: 0,
				NextURLInterval: 0,
				displayLastUpdate: false,
				displayLastUpdateFormat: 'ddd - HH:mm:ss',
				url: ["https://video.nest.com/embedded/live/wSbs3mRsOF?autoplay=1"],
				scrolling: "no",
				shutoffDelay: 10000 
			    }
		},
		{
			    module: 'SmartWebDisplay', // third cam
			    position: 'left', // same pos
			    config: {
				logDebug: false,
				height: "50%", 
				width: "100%", 
				updateInterval: 0,
				NextURLInterval: 0,
				displayLastUpdate: false,
				displayLastUpdateFormat: 'ddd - HH:mm:ss',
				url: ["https://video.nest.com/embedded/live/rsStd5iVLE?autoplay=1"], 
				scrolling: "no",
				shutoffDelay: 10000 
			    }
		},

		// **************************************************************************************** SECOND PAGE: radio module

		{
			module: "MplayerRadio",
			// header: "Radio",
			position: "bottom_left",
			config: {
				// customCommand: "modules/MplayerRadio/scripts/vlcWrapper.bash",
				// customCommandArgs: ["###URL###"],
				showControls: false,
				showTitles: true,
				stopOnSuspend: true,
				// autoplay: 0,
				stations: [
					{
						title: "Antena 1",
						url: "https://www.surfmusik.de/m3u/antena-1,579.m3u",
						logo: "https://upload.wikimedia.org/wikipedia/commons/thumb/2/26/Antena_1_blue_logo.png/800px-Antena_1_blue_logo.png?20200302000853",
					}
				],
			},
		},

		// **************************************************************************************** FOURTH PAGE: yt module		

		{
			module: 'YouTubeWebView',
			position: 'left', 
			config: {
				
				video_id: "gmW4TqOybzA", // IDs
				video_list: [
					"CtVyl402W5s", 
					"wmUVy43tqw4",
					"J_TxPQKcG7w", 
					"avWZsKyuRVE",
					"IpkIGGJMHBA", 
					"B9zgwx6mhrk",
					"6KvTDeHlIfI"
					],
				autoplay: false,
				controls: false,
				loop: true,
				modestbranding: true,
				width: "400px",
				height: "225px",
				referrer: "http://your.public.domain.org",
				},
		},

		// **************************************************************************************** FOURTH PAGE: fpolis bus times module

		{
			module: "BusTimes",
			position: "top_right",
		},


		// **************************************************************************************** END
	]
};

/*************** DO NOT EDIT THE LINE BELOW ***************/
if (typeof module !== "undefined") { module.exports = config; }
