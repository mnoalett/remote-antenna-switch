let networks;
let selectedNetwork;

function handleNetworkChange(element) {
  selectedNetwork = networks[element.selectedIndex - 1];
  console.log("selected " + selectedNetwork.ssid);
  if (selectedNetwork.encryption == "open") {
    document.getElementById("pass").setAttribute("disabled", true);
  } else {
    document.getElementById("pass").removeAttribute("disabled");
  }
}

function getFormValues() {
  return {
    password: document.getElementById("pass").value,
    dhcp: document.getElementById("dhcp").value,
    ip: document.getElementById("ip").value,
    gateway: document.getElementById("gateway").value,
    netmask: document.getElementById("nermask").value,
  };
}

function doReboot() {
  fetch("/reboot")
    .then(() => alert("Rebooting... This may take a few seconds"))
    .catch((error) => alert("Error"));
}

function doUpdate() {
  window.location = "/update";
}

function handleTabChange(evt, tab) {
  let i, tabcontent, tablinks;

  // Get all elements with class="tabcontent" and hide them
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }

  // Get all elements with class="tablinks" and remove the class "active"
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }

  // Show the current tab, and add an "active" class to the button that opened the tab
  document.getElementById(tab).style.display = "block";
  evt.currentTarget.className += " active";
}

async function handleRelayChange(checkbox) {
  let status = checkbox.checked;
  console.log(`set relay to ${status}`);
  const response = await fetch(`/api/relay`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ state: status ? "on" : "off" }),
  });
}

async function getRelayStatus() {
  const response = await fetch("/api/relay");
  const status = await response.json();
  return status;
}

function deleteNetworkInfo() {
  let text =
    "Are you sure to remove the Wi-Fi newtork\nThe device will reboot in AP mode";
  if (confirm(text) == true) {
    fetch("/api/config", {
      method: "DELETE",
    })
      .then(() => alert("Success! The device is rebooting in AP mode"))
      .catch((error) => alert("Error removing WiFi info"));
  }
}

function getIpAddress(name) {
  let ipAddress = "";
  let ipFields = document.querySelectorAll(`input[name^="${name}"]`);

  ipFields.forEach((ip, index) => {
    if (index > 0) ipAddress += ".";
    ipAddress += ip.value;
  });

  return ipAddress;
}

function save() {
  let params = {
    ssid: selectedNetwork.ssid,
    pass: document.getElementById("pass").value,
    dhcp: document.getElementById("dhcp").checked,
    ip: getIpAddress("ip"),
    gateway: getIpAddress("gw"),
    netmask: getIpAddress("nm"),
  };

  fetch("/api/config", {
    method: "POST",
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
    body: new URLSearchParams(params),
  })
    .then((res) =>
      alert(
        "Credentials saved! Wait the device to reboot. Once rebooted the device will try to connect to the network"
      )
    )
    .catch((error) => {
      alert("Error saving credentials");
      console.error("Error: " + err.message);
    });
}

function updateNetworkSettings() {
  let params = {
    dhcp: document.getElementById("dhcp").checked,
    ip: getIpAddress("ip"),
    gateway: getIpAddress("gw"),
    netmask: getIpAddress("nm"),
  };

  fetch("/api/config", {
    method: "POST",
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
    body: new URLSearchParams(params),
  })
    .then((res) => alert("Settings saved! The device will reboot"))
    .catch((error) => {
      alert("Error saving settings");
      console.error("Error: " + err.message);
    });
}

// Function to toggle the visibility of IP configuration fields
function toggleIPConfig() {
  let dhcpRadio = document.getElementById("dhcp");
  let fields = document.querySelectorAll(".ip-address");

  fields.forEach((field) => {
    if (dhcpRadio.checked) {
      field.setAttribute("disabled", true);
    } else {
      field.removeAttribute("disabled");
    }
  });
}

async function getConfig() {
  const response = await fetch("/api/config");
  const config = await response.json();
  return config;
}

async function getWifiNetworks() {
  document.querySelector('input[id = "scan"]').setAttribute("disabled", true);
  const res = await fetch("/wifi-networks");
  const wifiNetworks = await res.json();
  networks = wifiNetworks.networks;
  document.querySelector('input[id = "scan"]').removeAttribute("disabled");
  let selectTag = document.querySelector(".network-select");
  wifiNetworks.networks.map((net, i) => {
    let opt = document.createElement("option");
    opt.value = i; // the index
    opt.innerHTML = `${net.ssid} (${net.encryption}, RSSI: ${net.rssi})`;
    selectTag.append(opt);
  });
}

function formatUptime(milliseconds) {
  const days = Math.floor(milliseconds / (1000 * 60 * 60 * 24));
  const hours = Math.floor((milliseconds % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
  const minutes = Math.floor((milliseconds % (1000 * 60 * 6)) / (1000 * 60));

  return `${days}d ${hours}h ${minutes}m`;
}