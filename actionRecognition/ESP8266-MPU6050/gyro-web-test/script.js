let scene, camera, rendered, cube, arrow;
//{"gX":10,"gY":10,"gZ":10,"aX":10,"aY":0,"aZ":0,"tp":0}

let pitch = 0;
let roll  = 0;
let yaw   = 0;
		
let pitchOffset = 0;
let yawOffset = 0;
let rollOffset = 0;

let aX = 0;
let aY = 0;
let aZ = 0;

let aXOffset = 0;
let aYOffset = 0;
let aZOffset = 0;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  camera = new THREE.PerspectiveCamera(100, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);

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
  camera.position.z = 4;
  
  //init arrow
  var sourcePosition = new THREE.Vector3(0, 0, 0);
  var targetPosition = new THREE.Vector3(0, 0, 0);
  var direction = sourcePosition.clone().sub(targetPosition);
  var arrowLength = direction.length();
  arrow = new THREE.ArrowHelper(direction.normalize(), sourcePosition, arrowLength, 'green');
  //arrow.line.material.linewidth = 5;
  scene.add(arrow);
	
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

function startConnect() {
	//var host = "6f2bddbb318d4bc3b9496192a5073062.s1.eu.hivemq.cloud";
	//var port = 8884;
	//var host = "test.mosquitto.org";
	var host = "5.196.95.208";
	var port = 8080;
	//var host = "192.168.1.9";
	//var port = 9001;
	
	clientID = parseInt(Math.random() * 1000) + "-clientID";

	// Create a client instance
	client = new Paho.MQTT.Client(host, Number(port), clientID);
	//client.tls_insecure_set(true);
	
	// set callback handlers
	client.onConnectionLost = onConnectionLost;
	client.onMessageArrived = onMessageArrived;

	// connect the client
	client.connect({
		onSuccess: onConnect,
		/*userName:  "hivemax",
		password:  "HivePwd1"*/
	});
}

function startDisconnect() {
    client.disconnect();
}

// called when the client connects
function onConnect() {
  // Once a connection has been made, make a subscription and send a message.
  console.log("onConnect");
  client.subscribe("esp8266/test-max");
  
  var x = 0;
  var y = 0;
  var z = 0;
}

// called when the client loses its connection
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log("onConnectionLost:"+responseObject.errorMessage);
  }
}

// called when a message arrives
function onMessageArrived(message) {
    try {
		console.log("onMessageArrived:"+message.payloadString);
  
		var obj = JSON.parse(message.payloadString);
		
		//document.getElementById("deviceId").innerHTML = obj.id;
		
		document.getElementById("gyroX").innerHTML = obj.gX;
		document.getElementById("gyroY").innerHTML = obj.gY;
		document.getElementById("gyroZ").innerHTML = obj.gZ;
		
		document.getElementById("accX").innerHTML = obj.aX;
		document.getElementById("accY").innerHTML = obj.aY;
		document.getElementById("accZ").innerHTML = obj.aZ;

		document.getElementById("temp").innerHTML = obj.tp;
		
		// Change cube rotation after receiving the readinds
		pitch = THREE.Math.degToRad(obj.gX);
		roll  = THREE.Math.degToRad(obj.gY);
		yaw   = THREE.Math.degToRad(obj.gZ);
		
		//orientation cube
		cube.rotation.x = pitch+pitchOffset;
		cube.rotation.y = yaw+yawOffset; 
		cube.rotation.z = roll+rollOffset;
		
		//acceleration arrow
		var accThreshold = 0.05;
		if(Math.abs(aX-obj.aX) > accThreshold) aX = obj.aX;
		if(Math.abs(aY-obj.aY) > accThreshold) aY = obj.aY;
		if(Math.abs(aZ-obj.aZ) > accThreshold) aZ = obj.aZ;
		var sourcePosition = new THREE.Vector3(0, 0, 0);
		var targetPosition = new THREE.Vector3(aX+aXOffset, aZ+aZOffset, aY+aYOffset);
		targetPosition.applyAxisAngle(new THREE.Vector3( 1, 0, 0 ), pitch+pitchOffset);
		targetPosition.applyAxisAngle(new THREE.Vector3( 0, 1, 0 ), yaw+yawOffset);
		targetPosition.applyAxisAngle(new THREE.Vector3( 0, 0, 1 ), roll+rollOffset);
		var direction = sourcePosition.clone().sub(targetPosition);
		arrow.setDirection(direction.normalize());
		//arrow.setLength(direction.length()*4, 3, 1);
		arrow.setLength(direction.length()*4, direction.length()*4*0.3,  direction.length()*4*0.3*0.3);
		
		renderer.render(scene, camera);
	}
	catch(error) {
		console.log("error: ", error);
	}
}

//reset solid position
function resetPosition() {
	pitchOffset = -pitch;
	yawOffset = -yaw;
	rollOffset = -roll;
}

//reset acceleration
function resetAcceleration() {
	aXOffset = -aX;
	aYOffset = -aY;
	aZOffset = -aZ;
}

startConnect();
