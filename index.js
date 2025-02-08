// 부트스트랩 로더를 사용하여 Maps JavaScript API를 로드
// 부트스트랩 로더는 로딩을 위해 Maps JavaScript API를 준비합니다(importLibrary()가 호출될 때까지 라이브러리가 로드되지 않습니다).
(g => { var h, a, k, p = "The Google Maps JavaScript API", c = "google", l = "importLibrary", q = "__ib__", m = document, b = window; b = b[c] || (b[c] = {}); var d = b.maps || (b.maps = {}), r = new Set, e = new URLSearchParams, u = () => h || (h = new Promise(async (f, n) => { await (a = m.createElement("script")); e.set("libraries", [...r] + ""); for (k in g) e.set(k.replace(/[A-Z]/g, t => "_" + t[0].toLowerCase()), g[k]); e.set("callback", c + ".maps." + q); a.src = `https://maps.${c}apis.com/maps/api/js?` + e; d[q] = f; a.onerror = () => h = n(Error(p + " could not load.")); a.nonce = m.querySelector("script[nonce]")?.nonce || ""; m.head.append(a) })); d[l] ? console.warn(p + " only loads once. Ignoring:", g) : d[l] = (f, ...n) => r.add(f) && u().then(() => d[l](f, ...n)) })({
    key: "AIzaSyCFVqzYVeXbGGxIYSBzB2dH8AGdK_fCPN0",
    v: "weekly",
    // Use the 'v' parameter to indicate the version to use (weekly, beta, alpha, etc.).
    // Add other bootstrap parameters as needed, using camel case.
});

import { MarkerClusterer } from "@googlemaps/markerclusterer";

// Initialize and add the map
let map;

async function initMap() {
    // Request needed libraries.
    const { Map, InfoWindow } = await google.maps.importLibrary("maps");
    const { AdvancedMarkerElement, PinElement } = await google.maps.importLibrary(
        "marker",
    );
    // const { Place } = await google.maps.importLibrary("places");

    const map = new Map(document.getElementById("map"), {
        zoom: 12,
        center: { lat: 34.84555, lng: -111.8035 },
        mapId: "4504f8b37365c3d0",
    });

    // Set LatLng and title text for the markers.
    const tourStops = [
        {
            position: { lat: 34.8791806, lng: -111.8265049 },
            title: "Boynton Pass",
        },
        {
            position: { lat: 34.8559195, lng: -111.7988186 },
            title: "Airport Mesa",
        },
        {
            position: { lat: 34.832149, lng: -111.7695277 },
            title: "Chapel of the Holy Cross",
        },
        {
            position: { lat: 34.823736, lng: -111.8001857 },
            title: "Red Rock Crossing",
        },
        {
            position: { lat: 34.800326, lng: -111.7665047 },
            title: "Bell Rock",
        },
    ];

    // Create an info window to share between markers.
    const infoWindow = new InfoWindow();

    // Create the markers.
    const markers = tourStops.map(({ position, title }, i) => {
        // A marker with a custom SVG glyph.
        const glyphImg = document.createElement("img");

        glyphImg.src =
            "https://developers.google.com/maps/documentation/javascript/examples/full/images/google_logo_g.svg";

        const pin = new PinElement({
            glyph: glyphImg, //`${i + 1}`,
            scale: 1.5,
        });
        const marker = new AdvancedMarkerElement({
            map,
            position,
            content: pin.element,
            title: `${i + 1}. ${title}`,
            gmpClickable: true,
        });

        // Add a click listener for each marker, and set up the info window.
        marker.addListener("click", ({ domEvent, latLng }) => {
            const { target } = domEvent;

            infoWindow.close();
            infoWindow.setContent(marker.title);
            infoWindow.open(marker.map, marker);
        });
    });

    // Add a marker clusterer to manage the markers.
    const markerCluster = new markerClusterer.MarkerClusterer({ markers, map });
}

initMap();