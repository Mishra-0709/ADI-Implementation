function saveUser(name, email, password) {
    const users = JSON.parse(localStorage.getItem('users')) || [];
    users.push({ name, email, password });
    localStorage.setItem('users', JSON.stringify(users));
}

function findUser(name, password) {
    const users = JSON.parse(localStorage.getItem('users')) || [];
    return users.find(user => user.name === name && user.password === password);
}

document.getElementById('register-form').addEventListener('submit', function (e) {
    e.preventDefault();
    const name = document.getElementById('reg-name').value;
    const email = document.getElementById('reg-email').value;
    const password = document.getElementById('reg-password').value;

    saveUser(name, email, password);
    alert('Registration successful! Please login.');
    document.getElementById('login-page').style.display = 'none';
    document.getElementById('login-page-2').style.display = 'flex';
});

document.getElementById('login-form').addEventListener('submit', function (e) {
    e.preventDefault();
    const name = document.getElementById('login-name').value;
    const password = document.getElementById('login-password').value;

    if (findUser(name, password)) {
        alert('Login successful!');
        document.getElementById('login-page-2').style.display = 'none';
        document.getElementById('data-page').style.display = 'flex';
        fetchVehicleData();
        setInterval(fetchVehicleData, 5000);
    } else {
        alert('Invalid credentials. Please try again.');
    }
});

function fetchVehicleData() {
    fetch('https://api.thingspeak.com/channels/2780486/feeds.json?results=1')
        .then(response => response.json())
        .then(data => {
            const feed = data.feeds[0];

            let speed = feed.field1 ? parseFloat(feed.field1) : null;
            let acceleration = feed.field2 ? parseFloat(feed.field2) : null;
            let temperature = feed.field3 ? parseFloat(feed.field3) : null;
            let location = feed.field4 ? feed.field4 : "N/A";

            const html = `
                <p>Speed (v): ${speed !== null ? speed.toFixed(2) : "N/A"} m/s</p>
                <p>Acceleration: ${acceleration !== null ? acceleration.toFixed(2) : "N/A"} m/s²</p>
                <p>Temperature: ${temperature !== null ? temperature.toFixed(2) : "N/A"} °C</p>
                <p>Location: ${location}</p>
            `;
            document.getElementById('vehicle-data').innerHTML = html;

            if (location !== "N/A") {
                const [latitude, longitude] = location.split(',').map(Number);
                renderMap(latitude, longitude);
            }

            if (speed !== null && speed > 7) {
                document.getElementById('vehicle-data').innerHTML += `
                    <p style="color: red; font-weight: bold;">
                        Collision predicted, please adjust speed!
                    </p>`;
            }
        })
        .catch(() => {
            document.getElementById('vehicle-data').innerHTML = '<p>Error fetching data</p>';
        });
}

function renderMap(latitude, longitude) {
    const mapElement = document.getElementById('map');
    const map = new google.maps.Map(mapElement, {
        center: { lat: latitude, lng: longitude },
        zoom: 15
    });
    new google.maps.Marker({
        position: { lat: latitude, lng: longitude },
        map: map,
        title: 'Vehicle Location'
    });
}

function triggerSOS() {
    alert('SOS triggered! Calling emergency number...');
    window.location.href = 'tel:112';
}
