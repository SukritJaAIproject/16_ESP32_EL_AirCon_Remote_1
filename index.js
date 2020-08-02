const functions = require('firebase-functions')
const admin = require('firebase-admin')
const moment = require('moment')
var serviceAccount = require("./permissions.json")
const forecast = require("./getWeather")
var distance = require('gps-distance')

// ----------------------------------------------------------------------
//storage
// var gcloud = require('google-cloud');
// var gcs = gcloud.storage({
//     projectId: 'database-test-277802',
//     keyFilename: './keyfile.json'
//   });
const {Storage} = require('@google-cloud/storage');
const storage = new Storage({ keyFilename: './keyfile.json' })
const bucketName = 'database-test-277802.appspot.com';
// const storage = gcs.bucket('database-test-277802.appspot.com')
// ----------------------------------------------------------------------

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: "https://database-test-277802.firebaseio.com",
    // storageBucket: "database-test-277802.appspot.com"
});

// -----------------------------
//storage
const fs = require('fs-extra')
const path = require('path')
const os = require('os')
// const fileBucket = object.bucket; // The Storage bucket that contains the file.
// const bucket = admin.storage().bucket(fileBucket);

// -----------------------------
const rtdb = admin.database()
var ref = rtdb.ref("nodemcu/status")
const db = admin.firestore()
const express = require('express')
const cors = require('cors');

//app => express app
app = express()
app.use(cors({
    origin: true
}))


app.get('', (req, res) => {
    forecast("Talingchan", (weatherObject, error) => {
        if (error) {
            res.send({
                errorMessage: error,
            })
        } else {
            (async () => {
                try {
                    const time = moment().utcOffset(7)
                    const year = time.year()
                    const month = time.month()
                    const weekday = time.weekday()
                    const hour = time.hour()
                    const minute = time.minute()
                    const dateOfMonth = time.date()
                    const timeNow = time.format('YYYY MM DD H:mm:ss').toString()
                    await db.collection('products').doc('/' + timeNow + '/').create({
                        placeName: weatherObject.city,
                        temp: weatherObject.temp,
                        feel: weatherObject.feel,
                        latitude: weatherObject.lat,
                        longitude: weatherObject.long,
                        outlook: weatherObject.weather,
                        minute: minute,
                        hour: hour,
                        dayOfWeek: weekday,
                        dateOfMonth: dateOfMonth,
                    })
                    res.send({
                        placeName: weatherObject.city,
                        temp: weatherObject.temp,
                        feel: weatherObject.feel,
                        latitude: weatherObject.lat,
                        longitude: weatherObject.long,
                        outlook: weatherObject.weather
                    });
                    return res.status(200).send();
                } catch (error) {
                    console.log(error)
                    return res.status(500).send(error)
                }
            })();
        }
    })
})

app.post('/api/create', (req, res) => {
    //use function without declaring
    (async () => {
        try {
            await db.collection('join_test').doc('/' + req.body.id + '/').create({
                name: req.body.name,
                description: req.body.description,
                price: req.body.price
            })
            return res.status(200).send();
        } catch (error) {
            console.log(error)
            return res.status(500).send(error)
        }
    })();
})
const timeNow = () => {
    const time = moment().utcOffset(7)
    return time.format('YYYY MM DD H:mm:ss').toString()
}

fromEsp32 = (req, res) => {
    //use function without declaring
    (async () => {
        try {
            const time = timeNow()
            await db.collection('test_user').doc('fromEsp32').set({
                time: time,
                co2: req.body.co2,
                RH: req.body.RH,
                Temp: req.body.temp,
                PIR: req.body.PIR,
                LDR: req.body.LDR,
                ACStatus: req.body.ACStatus,
            }, {
                merge: true
            })
            return res.status(200).send();
        } catch (error) {
            console.log(error)
            return res.status(500).send(error)
        }
    })();
}

combinedData = express()
combinedData.use(cors({
    origin: true
}))



combinedData.get('', (req, res) => {

    //use function without declaring
    (async () => {
        const homeDoc = db.collection('test_user').doc('homeLocation')
        let homeCoordinate = await homeDoc.get()
        var home = {
            lat: homeCoordinate.data().homeLatitude,
            lon: homeCoordinate.data().homeLongitude,
        };
        try {
            //---------------------------------------------------
            let outdoorTemp
            let placeName
            let feel
            forecast("Talingchan", (weatherObject, error) => {
                if (error) {
                    res.send({
                        errorMessage: error,
                    })
                } else {
                    (async () => {
                        try {
                            outdoorTemp = weatherObject.temp
                            placeName = weatherObject.city
                            feel = weatherObject.feel
                        } catch (error) {
                            console.log(error)
                            return res.status(500).send(error)
                        }
                    })();
                }
            })
            //---------------------------------------------------

            const time = timeNow()
            const document1 = db.collection('test_user').doc('fromEsp32')
            const document2 = db.collection('test_user').doc('fromMobile')
            let dataFromEsp32 = await document1.get()
            let dataFromMobile = await document2.get()
            var mobileLocation = {
                lat: dataFromMobile.data().latitude,
                lon: dataFromMobile.data().longitude
            }
            let distanceFromHome = distance(home.lat, home.lon, mobileLocation.lat, mobileLocation.lon)
            await db.collection('test_user/combinedData/arrangedByTS').doc(time).set({
                time: admin.firestore.FieldValue.serverTimestamp(),
                co2: dataFromEsp32.data().co2,
                RH: dataFromEsp32.data().RH,
                Temp: dataFromEsp32.data().Temp,
                PIR: dataFromEsp32.data().PIR,
                LDR: dataFromEsp32.data().LDR,
                ACStatus: dataFromEsp32.data().ACStatus,
                Latitude: dataFromMobile.data().latitude,
                Longitude: dataFromMobile.data().longitude,
                outdoorTemp: outdoorTemp,
                placeName: placeName,
                feel: feel,
                distanceFromHome: distanceFromHome,
                setFan : dataFromEsp32.data().setFan,
                setTemp : dataFromEsp32.data().setTemp,
             
            }, {
                merge: true
            })
            const document3 = db.collection('test_user').doc('input_data')
            let Past = await document3.get()
            await db.collection('test_user').doc('input_data').set({
                time: admin.firestore.FieldValue.serverTimestamp(),  
                'distanceFromHome(t-3)': distanceFromHome,
                'distanceFromHome(t-4)': Past.data()['distanceFromHome(t-3)'],
                'distanceFromHome(t-5)': Past.data()['distanceFromHome(t-4)'],
                'distanceFromHome(t-6)': Past.data()['distanceFromHome(t-5)'],
                'distanceFromHome(t-7)': Past.data()['distanceFromHome(t-6)'],
                'distanceFromHome(t-8)': Past.data()['distanceFromHome(t-7)'],
                'distanceFromHome(t-9)': Past.data()['distanceFromHome(t-8)'],
                'distanceFromHome(t-10)': Past.data()['distanceFromHome(t-9)'],
                'distanceFromHome(t-11)': Past.data()['distanceFromHome(t-10)'],
                'distanceFromHome(t-12)': Past.data()['distanceFromHome(t-11)'],
                'distanceFromHome(t-13)': Past.data()['distanceFromHome(t-12)'],
                'distanceFromHome(t-14)': Past.data()['distanceFromHome(t-13)'],        
             
            }, {
                merge: true
            })

            return res.status(200).send(dataFromEsp32.data().co2)
        } catch (error) {
            console.log(error)
            return res.status(500).send(error)
        }

    })();
  


})
// -------------------------------------------------------------
nodeControl = express()
nodeControl.use(cors({
    origin: true
}))

// var ref = rtdb.ref("nodemcu/status")
nodeControl.post('/nodemcu/postStatus', (req, res) => {

    var data = req.body
    ref.push(data, (err) => {
        if(err){
            res.send(err)
        }else{
            res.json({"message":"success", result:true})
        }
      });
})
//much faster than updating with firestore
nodeControl.put('/nodemcu/updateStatus', (req, res) => {
    var uid = "-MAPCsPueXDWFdx-hz6F"
    var data = req.body
    ref.child(uid).update(data, (err) => {
        if(err){
            res.send(err)
        }else{
            res.json({"message":"success", result:true})
        }
      });
})


    exports.RTDB2fs = functions.database.ref('/nodemcu/status/-MAPI3e3ReHksFxyPmQb').onWrite((change, context) => {
        const beforeData = change.before.val(); // data before the write
        const afterData = change.after.val(); // data after the write
        (async () => {
            try {
                await db.collection('test_user').doc('fromEsp32').set({
                        
                    ACStatus: afterData.ACStatus,
                    LDR: afterData.LDR,
                    PIR: afterData.PIR,
                    RH: afterData.RH,
                    co2: afterData.co2,
                    Temp: afterData.temp,
                    time: admin.firestore.FieldValue.serverTimestamp(),
                    setTemp: afterData.setTemp,
                    setFan: afterData.setFan
                }
                , {
                    merge: true
                })   
            } catch (error) {
                return response.status(500).send(error)
            }
            })()
       });

    exports.ACControl = functions.database.ref('/nodemcu/status/-MAPCsPueXDWFdx-hz6F').onWrite((change, context) => {
        const beforeData = change.before.val(); // data before the write
        const afterData = change.after.val(); // data after the write
        (async () => {
            try {
                await db.collection('test_user').doc('ACControl').set({
                    setTemp: afterData.T,
                    fanspeed: afterData.FS,
                    acStatus: afterData.AC,
                    time: admin.firestore.FieldValue.serverTimestamp()
                }
                , {
                    merge: true
                })   
            } catch (error) {
                return response.status(500).send(error)
            }
            })()
       });

// -------------------------------------------------------------
exports.updateNode = functions.firestore
.document('test_user/Test_rtdb')
.onUpdate((change, context) => {
  // Get an object representing the document
  // e.g. {'name': 'Marie', 'age': 66}
  const newValue = change.after.data();
  var uid = "-MAPCsPueXDWFdx-hz6F"
  ref.child(uid).update(newValue, (err) => {
    if(err){
        res.send(err)
    }else{
        res.json({"message":"success", result:true})
    }
  });
  (async () => {
    try {
        await db.collection('test_user').doc('Test_rtdb_2').set({
            ACStatus: newValue.ACStatus,
        }, {
            merge: true
        })   
    } catch (error) {
        return response.status(500).send(error)
    }
})()


});



// -------------------------------------------------------------
exports.nodeControl = functions.https.onRequest(nodeControl)
exports.fromExternalAPI = functions.https.onRequest(app)
exports.fromEsp32 = functions.https.onRequest(fromEsp32)
exports.combinedData = functions.https.onRequest(combinedData)

exports.randomNumber = functions.https.onRequest((request, response) => {
    const number = Math.round(Math.random() * 100)
    response.send(number.toString())
})



exports.test2 = functions.https.onRequest((request, response) => {
    var value = "no data"
    db.collection("users").get().then(
        //after done => return value to callback function
        collection => {
            collection.forEach(docs => {
                value += docs.id
                value += ' '

            })
            return response.send(value)
        }


        //must return or response to the user
    ).catch((e) => {
        return "error"
    })
})


exports.test3 = functions.https.onRequest((request, response) => {
    (async () => {
        try {
            const time = moment().utcOffset(7)
            let value = ''
            const document = db.collection("users")
            let collection = await document.get()
            let innerDoc = ''
            for (let docs of collection) {
                value += docs.id
                innerDoc = docs.data().test
                value += innerDoc
                promises.push(db.collection("users").doc("writeTest").set({
                        test: value
                    })
                    //  .where('arenaUid', '==', arenasElement)
                    //  .where('sportUid', '==', sportsElement)
                    //  .get()
                );
                //  await db.collection(`${docs.id}`).doc(time).set({
                //      result: docs.id
                //  })

            }

            return response.status(200).send(`value: ${value}`)

        } catch (error) {
            return response.status(500).send(error)
        }

    })()
})

//-----------------------------------------------------------------------------------------------------------------------
exports.test4 = functions.https.onRequest((request, response) => {
    var value = "no data"
    db.collection("users").get().then(
        //after done => return value to callback function
        collection => {
            collection.forEach(docs => {
                value += docs.id
                value += ' '

            })
            return response.send(value)
        }


        //must return or response to the user
    ).catch((e) => {
        return "error"
    })
})


//-------------------------------const json2csv = require("json2csv").parse
exports.createCSV = functions.https.onRequest((request, response) => {

    const data = db.collection("test_user").doc("combinedData").collection("arrangedByTS") //collection name ,documentid, subcollection
    return data.get().then((querySnapshot) => {
        let object = {}
        let jsondata = [];
        querySnapshot.forEach(doc => {
            object = doc.data();
            const time = doc.data().time._seconds
            const now = moment.unix(time).utcOffset(7)
            object['timestamp'] = now.format('YYYY-MM-DD H:mm:ss')
            object['date'] = now.format('DD/MM/YYYY')
            object['time'] = now.format('H:mm:ss')
            object['day of week'] = now.weekday()
            jsondata.push(object);

        });
        const csv = json2csv(jsondata);
        response.setHeader(
            "Content-disposition",
            "attachment; filename=files.csv" //file name.csv
        );
        response.set("Content-Type", "text/csv");
        return response.status(200).send(csv)
    }).catch((err) => {
        return console.log(err);
    });
})
exports.getLatestData = functions.https.onRequest((request, response) => {
    //limit => still return a collection but smaller
    const fileName = `reports.csv`
    const tempFilePath = path.join(os.tmpdir(), fileName)
    let object = {}
    let jsondata = []
 
 
    db.collection("test_user").doc("combinedData").collection("arrangedByTS").orderBy('time', 'desc').limit(16000).get().then(
        (querySnapshot) => {
            querySnapshot.forEach(documentSnapshot => {
                object = documentSnapshot.data()
                const time = documentSnapshot.data().time._seconds
                const now = moment.unix(time).utcOffset(7)
                object['time'] = now.format('YYYY-MM-DD H:mm')
                object['hourOfDay'] = now.hour()
                object['hourOfDay(sin)'] = Math.sin((now.hour()*Math.PI*2)/24)
                object['hourOfDay(cos)'] = Math.cos((now.hour()*Math.PI*2)/24)
                object['dayOfWeek'] = now.weekday()
                object['dayOfWeek(sin)'] = Math.sin((now.weekday()*Math.PI*2)/7)
                object['dayOfWeek(cos)'] = Math.cos((now.weekday()*Math.PI*2)/7)    

                jsondata.push(object)
            }
            )
            // const csv = json2csv(jsondata);
            let fields = ['time','hourOfDay','hourOfDay(sin)','hourOfDay(cos)','dayOfWeek','dayOfWeek(sin)','dayOfWeek(cos)','RH','feel',
            'co2',
            // 'Latitude','Longitude',
            'distanceFromHome','PIR','outdoorTemp','Temp','LDR','ACStatus','setFan','setTemp' ]
            const { Parser } = require('json2csv')
            const json2csvParser = new Parser({ fields })
            const csv = json2csvParser.parse(jsondata)
            //---------------------------------------------
            // const options = { fields }
            // const csv = json2csv(jsondata,  function (err, csv) {
            //     if (err) {
            //         throw err}
            //     return console.log(csv)},
            //     options)
            //---------------------------------------------
            response.setHeader(
                "Content-disposition",
                "attachment; filename=files.csv" //file name.csv
            );
            response.set("Content-Type", "text/csv")
            return csv
            //must return or response to the user
        }).
        then((csv)=>{          
            return fs.outputFile(tempFilePath, csv)
        }).
        then(()=>{
            return storage.bucket(bucketName).upload(tempFilePath)
        }).
        then(()=>{
            return response.status(200).send()
        }).
        catch((e) => {
        return "error"
    })
})

