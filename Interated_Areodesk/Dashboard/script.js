// ==========================================
// AeroDesk Dashboard
// Load Data from flights.json
// ==========================================


// ==========================================
// LOAD JSON DATA
// ==========================================

fetch("data/flights.json")
    .then(response => {

        // Check if file was loaded successfully
        if (!response.ok) {
            throw new Error("Unable to load flights.json");
        }

        return response.json();
    })

    .then(data => {

        console.log("AeroDesk JSON Data Loaded Successfully");
        console.log(data);


        // ======================================
        // AIRPORT INFORMATION
        // ======================================

        document.getElementById("airportName").textContent =
            data.airport.name;

        document.getElementById("airportLocation").textContent =
            data.airport.location;

        document.getElementById("airportCode").textContent =
            "Airport Code: " + data.airport.code;


        // ======================================
        // STATISTICS
        // ======================================

        document.getElementById("activeFlights").textContent =
            data.statistics.activeFlights;

        document.getElementById("passengers").textContent =
            data.statistics.passengers;

        document.getElementById("fuelRemaining").textContent =
            data.statistics.fuelRemaining;

        document.getElementById("runwayStatus").textContent =
            data.statistics.runwayStatus;


        // ======================================
        // FLIGHT TABLE
        // ======================================

        const tableBody =
            document.getElementById("flightTableBody");

        data.flights.forEach(flight => {

            const row = document.createElement("tr");

            row.innerHTML = `
                <td>${flight.flightId}</td>
                <td>${flight.airline}</td>
                <td>${flight.destination}</td>
                <td>${flight.status}</td>
                <td>${flight.passengers}</td>
            `;

            tableBody.appendChild(row);
        });


        // ======================================
        // SUCCESS MESSAGE
        // ======================================

        console.log("Airport data loaded.");
        console.log("Statistics loaded.");
        console.log("Flight records loaded.");

    })

    .catch(error => {

        console.error("AeroDesk Error:", error);

    });


// ==========================================
// FLIGHT SEARCH / FILTER
// ==========================================

const searchBox = document.getElementById("flightSearch");

searchBox.addEventListener("input", function () {

    const searchText = searchBox.value.toLowerCase();

    const rows =
        document.querySelectorAll("#flightTableBody tr");

    rows.forEach(row => {

        const rowText =
            row.textContent.toLowerCase();

        if (rowText.includes(searchText)) {

            row.style.display = "";

        } else {

            row.style.display = "none";

        }

    });

});


// ==========================================
// PROJECT EVIDENCE BUTTONS
// ==========================================

const evidenceButtons =
    document.querySelectorAll(".evidence-button");


// ==========================================
// PL EVIDENCE
// ==========================================

if (evidenceButtons[0]) {

    evidenceButtons[0].addEventListener("click", function () {

        alert(
            "PL - Programming Lab\n\n" +

            "Array:\n" +
            "Passenger Management\n\n" +

            "Linked List:\n" +
            "Flight Records\n\n" +

            "Operations:\n" +
            "Insert, Display and Traverse"
        );

    });

}


// ==========================================
// OOP EVIDENCE
// ==========================================

if (evidenceButtons[1]) {

    evidenceButtons[1].addEventListener("click", function () {

        alert(
            "OOP - Object Oriented Programming\n\n" +

            "Class:\n" +
            "Passenger\n\n" +

            "Concepts:\n" +
            "Objects and Constructors\n\n" +

            "Principle:\n" +
            "Encapsulation"
        );

    });

}


// ==========================================
// COA RESULTS
// ==========================================

if (evidenceButtons[2]) {

    evidenceButtons[2].addEventListener("click", function () {

        alert(
            "COA - Computer Organization and Architecture\n\n" +

            "CO1:\n" +
            "80386 Architecture\n\n" +

            "CO2:\n" +
            "80386 Assembly Operations\n\n" +

            "CO3:\n" +
            "Arithmetic / Fuel Calculation\n\n" +

            "Example:\n" +
            "Distance = 1200 km\n" +
            "Fuel Rate = 5 units/km\n" +
            "Fuel Required = 6000 units\n" +
            "Available Fuel = 10000 units\n" +
            "Remaining Fuel = 4000 units"
        );

    });

}


// ==========================================
// CG VISUALIZATION
// ==========================================

if (evidenceButtons[3]) {

    evidenceButtons[3].addEventListener("click", function () {

        alert(
            "CG - Computer Graphics\n\n" +

            "Visualization:\n" +
            "Airport Environment\n\n" +

            "Objects:\n" +
            "Runway, Aircraft and Control Tower\n\n" +

            "Technology:\n" +
            "OpenGL\n\n" +

            "Graphics Concepts:\n" +
            "Graphics Primitives and 2D Visualization"
        );

    });

}


// ==========================================
// DASHBOARD READY MESSAGE
// ==========================================

console.log("AeroDesk Dashboard JavaScript Loaded Successfully.");