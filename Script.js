const firebaseConfig = {
    apiKey: "AIzaSyBO7GDJQMNRyJ9mZegNo2MsCBPjt1p9LQQ",
    authDomain: "iot-lab-eeb2b.firebaseapp.com",
    databaseURL: "https://iot-lab-eeb2b-default-rtdb.firebaseio.com",
    projectId: "iot-lab-eeb2b",
    storageBucket: "iot-lab-eeb2b.appspot.com",
    messagingSenderId: "535887261426",
    appId: "1:535887261426:web:13a590e8886c0a20b7aa6b"
  };    //Khởi tạo các mã liên kết với cơ sở dữ liệu firebase

  // Initialize Firebase
    firebase.initializeApp(firebaseConfig);
    var database = firebase.database();   // Khai báo biến đối tượng đến cơ sở dữ liệu, lưu ý phải khai báo thư viên reactjs bên html để sử dụng các hành vi lấy gửi dữ liệu trên firebase


database.ref("/UsersData/KMK0IoANy5fwlOqPy5WBRf6XKiX2/readings/innertemp").on("value",function(snapshot) {    //biến đối tượng tham chiếu đến đường dẫn cơ sở dữ liệu
    var temp = snapshot.val();															//khai báo biến temp lấy giá trị khi có bất cứ thay đổi dữ liệu trong đường dẫn
    document.getElementById("TempIn").innerHTML = temp;					//gán biến temp vào trong html với cú pháp innerHTML với Id là tempIn nằm trong tag span ở html
});

database.ref("/UsersData/KMK0IoANy5fwlOqPy5WBRf6XKiX2/readings/outtertemp").on("value",function(snapshot) {
    var temp = snapshot.val();
    document.getElementById("TempOut").innerHTML = temp;
});

database.ref("/UsersData/KMK0IoANy5fwlOqPy5WBRf6XKiX2/readings/humid").on("value",function(snapshot) {
    var humid = snapshot.val();
    document.getElementById("Humid").innerHTML = humid;
});

database.ref("/UsersData/KMK0IoANy5fwlOqPy5WBRf6XKiX2/readings/gas").on("value",function(snapshot) {
    var gas = snapshot.val();
    document.getElementById("Gas").innerHTML = gas;
});



$(document).ready(function(){				// sử dụng jquery cũng tương tự
	var Fan_status;

	database.ref("/Web&App/Fan_status").on("value", function(snap){   // biến đối tượng tham chiếu đến đường dẫn firebase
		Fan_status = snap.val();									// cập nhật giá trị khi có bất cứ sự kiện nào xảy ra tại đường dẫn này
		if(Fan_status == "1"){    // check from the firebase		//kiểm tra biến
			//$(".Light1Status").text("The light is off");
			document.getElementById("unact").style.display = "none";	// if true thì switch on sẽ hiển thị và switch off sẽ tắt
			document.getElementById("act").style.display = "block";
		} else {
			//$(".Light1Status").text("The light is on");
			document.getElementById("unact").style.display = "block"; // if false thì ngược lại
			document.getElementById("act").style.display = "none";
		}
	});

    $(".toggle-btn").click(function(){
		var firebaseRef = firebase.database().ref().child("/Web&App/Fan_status");   // gửi dữ liệu lên firebase, tương tự khai báo biến đối tượng tham chiếu đến đường dẫn
		if(Fan_status == "1"){    // post to firebase
			firebaseRef.set("0");    // hàm gửi data lên cơ sở dữ liệu
			Fan_status = "0";		// đồng thời cập nhật lại giá trị của biến fan_status đã khai báo ở trên
		} else {
			firebaseRef.set("1");
			Fan_status = "1";
		}
	})
});


window.onload = function() {     // hàm load trang, khi load xong trang web thì các hành vi trong hàm mới được tương tác với user
	var slider = document.getElementById("myRange");			//biến slider tham chiếu đến thuộc tính giá trị thumb hiện tại của slider
	var output = document.getElementById("demo");				//biến output tham chiếu đến thuộc tính demo của giá trị hiển thị bên dưới slider
	output.innerHTML = slider.value; // Display the default slider value
  
	// Update the current slider value (each time you drag the slider handle)
	slider.oninput = function() {       //khi có bất kỳ thay đổi liên quan đến thanh trượt thì
	  output.innerHTML = this.value;   //biến output gán với giá trị ở ngay tại điểm đó
	  var firebaseRef = firebase.database().ref().child("/Web&App/Speed");   //tham chiếu đường dẫn cơ sở dữ liệu
  	  firebaseRef.set(this.value);			//gửi dữ liệu lên
	  var value = (this.value-this.min)/(this.max-this.min)*100     //tính giá trị % thay đổi của slider hiện tại so với giá trị nhỏ nhất, tại sao?
      this.style.background = 'linear-gradient(to right, #13aabe 0%, #13aabe ' + value + '%, #d3d3d3 ' + value + '%, #d3d3d3 100%)' // vì tỷ lệ phần trăm đó sẽ hiển thị tỷ lệ background ở phía sau cái thumb khi kéo
	}
  }


// Listen for changes to the value in the database
database.ref("/Web&App/Speed").on('value', function(snapshot) {      //hàm này tương tác cho sự thay đổi của slider theo giá trị của firebase
  // Get the new value from the snapshot
  var newValue = snapshot.val();				//lấy giá trị slider trên firebase về
  // Update the value of the slider
  var slider = document.getElementById("myRange");			//tham chiếu biến slider đến id myrange của tag input html
  slider.value = newValue;							//gán giá trị slider vừa lấy trên firebase gán cho biến slider
  // Update the displayed value
  var output = document.getElementById("demo");		// đồng thời cập nhật lun giá trị output hiển thị phía trên thanh slider
  output.innerHTML = newValue;

  var value = (newValue-slider.min)/(slider.max-slider.min)*100; // cũng đồng thời cập nhật % hiển thị background
  slider.style.background = 'linear-gradient(to right, #13aabe 0%, #13aabe ' + value + '%, #d3d3d3 ' + value + '%, #d3d3d3 100%)';
});

  