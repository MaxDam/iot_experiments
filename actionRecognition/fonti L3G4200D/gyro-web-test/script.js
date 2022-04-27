let scene, camera, rendered, cube;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  camera = new THREE.PerspectiveCamera(75, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

  document.getElementById('3Dcube').appendChild(renderer.domElement);

  // Create a geometry
  const geometry = new THREE.BoxGeometry(5, 1, 4);

  // Materials of each face
  var cubeMaterials = [
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
  ];

  const material = new THREE.MeshFaceMaterial(cubeMaterials);

  cube = new THREE.Mesh(geometry, material);
  scene.add(cube);
  camera.position.z = 5;
  renderer.render(scene, camera);
}

// Resize the 3D object when the browser window changes size
function onWindowResize(){
  camera.aspect = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"));
  //camera.aspect = window.innerWidth /  window.innerHeight;
  camera.updateProjectionMatrix();
  //renderer.setSize(window.innerWidth, window.innerHeight);
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

}

window.addEventListener('resize', onWindowResize, false);

// Create the 3D representation
init3D();

// Create a client instance
client = new Paho.MQTT.Client("broker.hivemq.com", 8000, "clientId");
//client = new Paho.MQTT.Client("broker.hivemq.com", 1883, "clientId");

// set callback handlers
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

// connect the client
client.connect({
	onSuccess:onConnect
});

// called when the client connects
function onConnect() {
  // Once a connection has been made, make a subscription and send a message.
  console.log("onConnect");
  client.subscribe("/test-max");
  
  var x = 0; //Math.floor(Math.random() * 200)
  var y = 0; //Math.floor(Math.random() * 200)
  var z = 0; //Math.floor(Math.random() * 200)
  
  message = new Paho.MQTT.Message("{\"gyroX\":\""+x+"\",\"gyroY\":\""+y+"\",\"gyroZ\":\""+z+"\"}");
  message.destinationName = "/test-max";
  client.send(message);
}

// called when the client loses its connection
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log("onConnectionLost:"+responseObject.errorMessage);
  }
}

// called when a message arrives
function onMessageArrived(message) {
    console.log("onMessageArrived:"+message.payloadString);
  
    try {
		var obj = JSON.parse(message.payloadString);
		document.getElementById("gyroX").innerHTML = obj.gyroX;
		document.getElementById("gyroY").innerHTML = obj.gyroY;
		document.getElementById("gyroZ").innerHTML = obj.gyroZ;
		
		// Change cube rotation after receiving the readinds
		cube.rotation.x = obj.gyroY;
		cube.rotation.z = obj.gyroX;
		cube.rotation.y = obj.gyroZ;
		renderer.render(scene, camera);
		
		/*
		document.getElementById("temp").innerHTML = obj.temperature;
		
		document.getElementById("accX").innerHTML = obj.accX;
		document.getElementById("accY").innerHTML = obj.accY;
		document.getElementById("accZ").innerHTML = obj.accZ;
		*/
	}
	catch(error) {
		console.log("error: ", error);
	}
}
