var DOMAIN = "www.domain.com/pathToFiles";
var WUAPIKEY = "wunderground api key";
var OWMAPIKEY = "openweathermap api key";

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function lookupPremiumKey(key) {
    var url = "http://" + DOMAIN + "/keys.txt";
    console.log("Lookup key '" +key + "'");
    
    // Send request to server
    xhrRequest(url, 'GET',
               function(responseText) {
               console.log("Lookup key response " + responseText);
               if (!responseText || responseText == '')
               {
                 console.log("Lookup key FAILED");
                 return;
               }
               
               var json = JSON.parse(responseText);
               var found = 0;
               var foundKey = 'not found';
               
               for (i = 0; found == 0 && i < json.keys.length; i++) {
                   var index = key.indexOf(json.keys[i]);
                   console.log("indexOf(" + json.keys[i] + ") = " + index);
                   if (index >= 0) {
                       found = 1;
                       foundKey = json.keys[i];
                       console.log("found key " + i + " " + json.keys[i]);
                   }
               }
               
               var dictionary = {
                 "KEY_PREMIUM_KEY_VALID": found,
               };
               
               localStorage.setItem('settings', '{"purchasekeyvalid":' + found + '}');
               var settings = localStorage.getItem('settings');
               console.log("stored settings: " + JSON.stringify(settings));
               
               // Send to Pebble
               Pebble.sendAppMessage(dictionary,
                                     function(e) {
                                       console.log("Lookup key sent to Pebble successfully!");
                                     },
                                     function(e) {
                                       console.log("Error sending Lookup key to Pebble!");
                                     }
                                     );
               
               }
               );
}

var weatherSrc = 0;
var latitude = 0;
var longitude = 0;
var language = '';
const ERROR_NONE                 =  0;
const ERROR_GET_LOCATION         =  1;
const ERROR_GET_WEATHER          =  2;
const ERROR_GET_WEATHER_FORECAST =  3;

function getWeatherFromWeatherUnderground(useCustomLocation) {
    // Construct URL
    
    if (language == '') language = 'EN';
    
    var url = "http://api.wunderground.com/api/" + WUAPIKEY + "/conditions/forecast/lang:" + language + "/q/";
    if (useCustomLocation) {
        var options = JSON.parse(localStorage.getItem('options'));
        url += options.weatherLocation;
    } else {
        url += + latitude + "," + longitude;
    }
    url += ".json";
    console.log("get WeatherUnderground forecast " + url);
    
    // Send request to server
    xhrRequest(url, 'GET',
        function(responseText) {
               console.log("get WeatherUnderground forecast response " + responseText);
               if (!responseText || responseText == '')
               {
                    console.log("get WeatherUnderground forecast FAILED");
                    Pebble.sendAppMessage({ "KEY_ERROR": ERROR_GET_WEATHER },
                                         function(e) {
                                         console.log("ERROR_GET_WEATHER sent to Pebble successfully!");
                                         },
                                         function(e) {
                                         console.log("Error sending ERROR_GET_WEATHER to Pebble!");
                                         }
                                         );
                    return;
               }
               
               // responseText contains a JSON object with weather info
               var json = JSON.parse(responseText);
               if (json.response.error !== undefined && json.response.error !== null) {
                   console.log("WeatherUnderground forecast FAILED with ERROR " + json.response.error.description);
                   Pebble.sendAppMessage({ "KEY_ERROR": ERROR_GET_WEATHER },
                                         function(e) {
                                         console.log("ERROR_GET_WEATHER sent to Pebble successfully!");
                                         },
                                         function(e) {
                                         console.log("Error sending ERROR_GET_WEATHER to Pebble!");
                                         }
                                         );
                   return;
               }
               
               var conditions = json.current_observation;
               var forecast = json.forecast.simpleforecast;
               var dictionary = {
                   "KEY_TEMPERATURE": Math.round(conditions.temp_c * 100),
                   "KEY_CONDITIONS": conditions.weather,
                   "KEY_COND_ICON": conditions.icon,
                   "KEY_FORECAST_DAY0_TEMP_MIN": Math.round(forecast.forecastday[0].low.celsius * 100),
                   "KEY_FORECAST_DAY0_TEMP_MAX": Math.round(forecast.forecastday[0].high.celsius * 100),
                   "KEY_FORECAST_DAY0_ICON": forecast.forecastday[0].icon,
                   "KEY_FORECAST_DAY1_TEMP_MIN": Math.round(forecast.forecastday[1].low.celsius * 100),
                   "KEY_FORECAST_DAY1_TEMP_MAX": Math.round(forecast.forecastday[1].high.celsius * 100),
                   "KEY_FORECAST_DAY1_ICON": forecast.forecastday[1].icon,
                   "KEY_FORECAST_DAY2_TEMP_MIN": Math.round(forecast.forecastday[2].low.celsius * 100),
                   "KEY_FORECAST_DAY2_TEMP_MAX": Math.round(forecast.forecastday[2].high.celsius * 100),
                   "KEY_FORECAST_DAY2_ICON": forecast.forecastday[2].icon,
                   "KEY_FORECAST_DAY3_TEMP_MIN": Math.round(forecast.forecastday[3].low.celsius * 100),
                   "KEY_FORECAST_DAY3_TEMP_MAX": Math.round(forecast.forecastday[3].high.celsius * 100),
                   "KEY_FORECAST_DAY3_ICON": forecast.forecastday[3].icon,
                   "KEY_CITY": conditions.display_location.city,
               };

               // Send to Pebble
               Pebble.sendAppMessage(dictionary,
                                     function(e) {
                                        console.log("WeatherUnderground forecast sent to Pebble successfully!");
                                     },
                                     function(e) {
                                        console.log("Error sending WeatherUnderground forecast to Pebble!");
                                     }
                                     );
               
               }
               );
}

function getForecastWeatherFromOpenWeather() {
    // Construct URL
    var url = "http://api.openweathermap.org/data/2.5/forecast/daily?cnt=4&lat=" + latitude + "&lon=" + longitude + "&appid=" + OWMAPIKEY;
    console.log("get OpenWeather forecast " + url);
    
    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET',
               function(responseText) {
               console.log("get OpenWeather forecast response " + responseText);
               if (!responseText || responseText == '' || responseText[0] !== '{')
               {
               console.log("get OpenWeather forecast FAILED");
               Pebble.sendAppMessage({ "KEY_ERROR": ERROR_GET_WEATHER_FORECAST },
                                     function(e) {
                                     console.log("ERROR_GET_WEATHER  sent to Pebble successfully!");
                                     },
                                     function(e) {
                                     console.log("Error sending ERROR_GET_WEATHER to Pebble!");
                                     }
                                     );
               return;
               }
               
               var json = JSON.parse(responseText);
               var dictionary = {
                   "KEY_FORECAST_DAY0_TEMP_MIN": Math.round((json.list[0].temp.min - 273.15) * 100),
                   "KEY_FORECAST_DAY0_TEMP_MAX": Math.round((json.list[0].temp.max - 273.15) * 100),
                   "KEY_FORECAST_DAY0_ICON": json.list[0].weather[0].icon,
                   "KEY_FORECAST_DAY1_TEMP_MIN": Math.round((json.list[1].temp.min - 273.15) * 100),
                   "KEY_FORECAST_DAY1_TEMP_MAX": Math.round((json.list[1].temp.max - 273.15) * 100),
                   "KEY_FORECAST_DAY1_ICON": json.list[1].weather[0].icon,
                   "KEY_FORECAST_DAY2_TEMP_MIN": Math.round((json.list[2].temp.min - 273.15) * 100),
                   "KEY_FORECAST_DAY2_TEMP_MAX": Math.round((json.list[2].temp.max - 273.15) * 100),
                   "KEY_FORECAST_DAY2_ICON": json.list[2].weather[0].icon,
                   "KEY_FORECAST_DAY3_TEMP_MIN": Math.round((json.list[3].temp.min - 273.15) * 100),
                   "KEY_FORECAST_DAY3_TEMP_MAX": Math.round((json.list[3].temp.max - 273.15) * 100),
                   "KEY_FORECAST_DAY3_ICON": json.list[3].weather[0].icon,
                   "KEY_CITY": json.city.name,
               };
               
               Pebble.sendAppMessage(dictionary,
                                     function(e) {
                                        console.log("OpenWeather forecast sent to Pebble successfully!");
                                     },
                                     function(e) {
                                        console.log("Error sending OpenWeather forecast to Pebble!");
                                     }
                                     );
               
               }
               );
}

function getCurrentWeatherFromOpenWeather() {
    // Construct URL
    var url = "http://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longitude + "&appid=" + OWMAPIKEY;
    console.log("get current OpenWeather " + url);
    
    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET',
               function(responseText) {
                   console.log("get current OpenWeather response " + responseText);
               
                    if (!responseText || responseText == '' || responseText[0] !== '{') {
                    console.log("get current OpenWeather FAILED");
                        Pebble.sendAppMessage({ "KEY_ERROR": ERROR_GET_WEATHER },
                                     function(e) {
                                     console.log("current OpenWeather sent to Pebble successfully!");
                                     },
                                     function(e) {
                                     console.log("Error sending ERROR_GET_WEATHER to Pebble!");
                                     }
                                     );

                    return;
                   }
               
                   var json = JSON.parse(responseText);
                   var dictionary = {
                       "KEY_TEMPERATURE": Math.round((json.main.temp - 273.15) * 100),
                       "KEY_CONDITIONS": json.weather[0].main,
                       "KEY_COND_ICON": json.weather[0].icon
                   };
                   
                   // Send to Pebble
                   Pebble.sendAppMessage(dictionary,
                                         function(e) {
                                            console.log("OpenWeather current sent to Pebble successfully!");
                                            getForecastWeatherFromOpenWeather();
                                         },
                                         function(e) {
                                            console.log("Error sending OpenWeather current to Pebble!");
                                         }
                                         );
                   }
                );
}

function locationSuccess(pos) {
    latitude = pos.coords.latitude;
    longitude = pos.coords.longitude;
    localStorage['latitude'] = latitude;
    localStorage['longitude'] = longitude;
    
    if (weatherSrc == 0) {
        getCurrentWeatherFromOpenWeather();
    } else if (weatherSrc == 1){
        getWeatherFromWeatherUnderground(false);
    }
}

function locationError(err) {
    console.log("Error requesting location!");
    Pebble.sendAppMessage({ "KEY_ERROR": ERROR_GET_LOCATION },
                          function(e) {
                          console.log("ERROR_GET_LOCATION sent to Pebble successfully!");
                          },
                          function(e) {
                          console.log("Error sending ERROR_GET_LOCATION to Pebble!");
                          }
                          );
    
    if (latitude !== null && latitude !== undefined && latitude !== 0 && longitude !== null && longitude !== undefined && longitude !== 0) {
         if (weatherSrc == 0) {
             getCurrentWeatherFromOpenWeather();
         } else {
             getWeatherFromWeatherUnderground(false);
         }
    }
}

function getWeather() {
    console.log("get location");

  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000, enableHighAccuracy: true}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");
 
    if (localStorage['latitude'] != null) {
        latitude = localStorage['latitude'];
        longitude = localStorage['longitude'];
    }
    console.log("location " + latitude + "," + longitude);
    
    var options = JSON.parse(localStorage.getItem('options'));
    if (options !== null) {
        if (options.language) {
            language = options.language;
        }
    }
  });

const REQUEST_WEATHER            = 0;
const REQUEST_PREMIUM_KEY_LOOKUP = 1;

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("Received message: " + JSON.stringify(e.payload));
                        
    if (e.payload.KEY_REQUEST == REQUEST_WEATHER) {
        // Get weather info
        weatherSrc = e.payload.KEY_WEATHER_SOURCE;
        if (weatherSrc == 2) {
            getWeatherFromWeatherUnderground(true);
        } else {
            getWeather();
        }
    } else if (e.payload.KEY_REQUEST == REQUEST_PREMIUM_KEY_LOOKUP) {
        // Validate premium key
        lookupPremiumKey(e.payload.KEY_PREMIUM_KEY);
    }
  });

Pebble.addEventListener("showConfiguration", function(e) {
    console.log("showConfiguration");
    var uri = "http://" + DOMAIN + "/configurable_aplite7.html";
    if(Pebble.getActiveWatchInfo) {
        var watch = Pebble.getActiveWatchInfo();
        if (watch) {
            console.log("platform: " + watch.platform);
            if (watch.platform == "basalt") {
                uri = "http://" + DOMAIN + "/configurable_basalt7.html";
            } else if (watch.platform == "chalk") {
                uri = "http://" + DOMAIN + "/configurable_chalk7.html";
            } else if (watch.platform == "diorite") {
                uri = "http://" + DOMAIN + "/configurable_diorite7.html";
            }
        }
    }
    var options = JSON.parse(localStorage.getItem('options'));
    console.log("read options: " + JSON.stringify(options));
 
    if (options !== null && options.tempformat) {
        uri = uri + '?tempformat=' + encodeURIComponent(options.tempformat)
        + '&face=' + encodeURIComponent(options.face)
        + '&hands=' + encodeURIComponent(options.hands)
        + '&handcolor=' + encodeURIComponent(options.handcolor)
        + '&facecolor=' + encodeURIComponent(options.facecolor)
        + '&digitime=' + encodeURIComponent(options.digitime)
        + '&weekday=' + encodeURIComponent(options.weekday)
        + '&bt=' + encodeURIComponent(options.bt)
        + '&battery=' + encodeURIComponent(options.battery)
        + '&seconds=' + encodeURIComponent(options.seconds)
        + '&date=' + encodeURIComponent(options.date)
        + '&weathertemp=' + encodeURIComponent(options.weathertemp)
        + '&weathercond=' + encodeURIComponent(options.weathercond)
        + '&purchasekey=' + encodeURIComponent(options.purchasekey)
        + '&wunderground=' + encodeURIComponent(options.wunderground)
        + '&language=' + encodeURIComponent(options.language)
        + '&timezone=' + encodeURIComponent(options.timezone)
        + '&dnd=' + encodeURIComponent(options.dnd)
        + '&dndStart=' + encodeURIComponent(options.dndStart)
        + '&dndEnd=' + encodeURIComponent(options.dndEnd)
        + '&houralert=' + encodeURIComponent(options.houralert)
        + '&btalert=' + encodeURIComponent(options.btalert)
        + '&batteryalert=' + encodeURIComponent(options.batteryalert);
        
        if (options.invert) uri += '&invert=' + encodeURIComponent(options.invert);
        if (options.fillhands) uri += '&fillhands=' + encodeURIComponent(options.fillhands);
        if (options.weatherfetch) uri += '&weatherfetch=' + encodeURIComponent(options.weatherfetch);
        if (options.debug) uri += '&debug=' + encodeURIComponent(options.debug);
        if (options.largeFonts) uri += '&largeFonts=' + encodeURIComponent(options.largeFonts);
        if (options.facebg) uri += '&facebg=' + encodeURIComponent(options.facebg);
        if (options.weatherLocation) uri += '&weatherLocation=' + encodeURIComponent(options.weatherLocation);
        if (options.shake) uri += '&shake=' + encodeURIComponent(options.shake);

        var settings = JSON.parse(localStorage.getItem('settings'));
        console.log("read settings: " + JSON.stringify(settings));

        if (settings !== null && settings.purchasekeyvalid) {
            uri = uri + '&purchasekeyvalid=' + encodeURIComponent(settings.purchasekeyvalid);
        }
    }
                        
    console.log("showing configuration");
    console.log(uri);
    Pebble.openURL(uri);
});

Pebble.addEventListener("webviewclosed",
                        function(e) {
    // webview closed
    console.log("webviewclosed");
    if (e.response !== '') {
        var json = JSON.parse(decodeURIComponent(e.response));
        console.log("storing options: " + JSON.stringify(json));
        localStorage.setItem('options', JSON.stringify(json));
               
        if (json.language) language = json.language;
                        
        var dictionary = {
            "KEY_TEMP_FORMAT": json.tempformat,
            "KEY_FACE_ID": json.face,
            "KEY_HANDS": json.hands,
            "KEY_DIGITIME": json.digitime,
            "KEY_INVERT": json.invert,
            "KEY_FILLHANDS": json.fillhands,
            "KEY_SHAKE": json.shake,
            "KEY_LARGE_FONTS": json.largeFonts,
            "KEY_DEBUG": json.debug,
            "KEY_WEEKDAY": json.weekday,
            "KEY_BT": json.bt,
            "KEY_BATTERY": json.battery,
            "KEY_SECONDS": json.seconds,
            "KEY_DATE": json.date,
            "KEY_WEATHERTEMP": json.weathertemp,
            "KEY_WEATHERCOND": json.weathercond,
            "KEY_WEATHER_FETCH": json.weatherfetch,
            "KEY_PREMIUM_KEY": json.purchasekey,
        };

        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
          function(e) {
            console.log("Configuration info sent to Pebble successfully!");
              var settings = JSON.parse(localStorage.getItem('settings'));
              if (settings !== null && settings.purchasekeyvalid) {
                  var dictionary = {
                      "KEY_PREMIUM_KEY_VALID": settings.purchasekeyvalid,
                      "KEY_PREMIUM_KEY": json.purchasekey,
                      "KEY_WEATHER_SOURCE": json.wunderground,
                      "KEY_WEATHER_LOCATION": json.weatherLocation,
                      "KEY_HOUR_ALERT": json.houralert,
                      "KEY_BT_ALERT": json.btalert,
                      "KEY_BATTERY_ALERT": json.batteryalert,
                      "KEY_DND": json.dnd,
                      "KEY_DND_START": json.dndStart,
                      "KEY_DND_END": json.dndEnd,
                      "KEY_LANG": json.language,
                      "KEY_TIMEZONE": json.timezone,
                      "KEY_FACE_COLOR": json.facecolor,
                      "KEY_HAND_COLOR": json.handcolor,
                      "KEY_FACE_BG": json.facebg,
                  };
                  
                  // Send to Pebble
                  Pebble.sendAppMessage(dictionary,
                    function(e) {
                      console.log("Configuration premium info sent to Pebble successfully!");
                    },
                    function(e) {
                      console.log("Error sending premium configuration info to Pebble!");
                    });
              } else {
                  var dictionary = {
                      "KEY_PREMIUM_KEY_VALID": 0,
                      "KEY_PREMIUM_KEY": json.purchasekey,
                      "KEY_LANG": "EN",
                  };
                  
                  // Send to Pebble
                  Pebble.sendAppMessage(dictionary,
                                        function(e) {
                                        console.log("Configuration premium reset sent to Pebble successfully!");
                                        },
                                        function(e) {
                                        console.log("Error sending premium configuration info to Pebble!");
                                        });
              }
          },
          function(e) {
            console.log("Error sending configuration info to Pebble!");
          }
        );
    } else {
        console.log("No options received.  Cancelled");
    }
});

