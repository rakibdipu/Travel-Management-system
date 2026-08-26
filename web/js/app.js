/**
 * Master Application Controller & UI Logic for Web Interface
 * Fully connected with SQLite3 Backend REST API and Local Fallback
 */

// Global State
let state = {
    currentUser: null,
    graph: new GraphNetwork(),
    packages: [],
    users: [],
    bookings: [],
    coupons: {},
    connections: [],
    activeCheckout: null,
    selectedOrigin: null,
    selectedDestination: null,
    activeDijkstraResult: null
};

// ============================================================================
// 0. SQLITE3 DATABASE CLIENT (REST API SYNC)
// ============================================================================
const dbSync = {
    isServerActive: false,

    async init() {
        try {
            const res = await fetch("/api/status");
            if (res.ok) {
                const data = await res.json();
                this.isServerActive = true;
                const badge = document.getElementById("dbStatusBadge");
                if (badge) {
                    badge.innerHTML = `<span class="w-2 h-2 rounded-full bg-eco-green animate-pulse"></span><span>SQLite3 DB (Live Sync)</span>`;
                    badge.classList.remove("hidden");
                }
                await this.bootstrapFromDatabase();
            }
        } catch (e) {
            console.log("[DB] Running in local storage mode");
            const badge = document.getElementById("dbStatusBadge");
            if (badge) {
                badge.innerHTML = `<span class="w-2 h-2 rounded-full bg-gold-400"></span><span>Local DB Ready</span>`;
                badge.classList.remove("hidden");
            }
        }
    },

    async bootstrapFromDatabase() {
        try {
            const res = await fetch("/api/data/bootstrap");
            if (res.ok) {
                const data = await res.json();
                if (data.users && data.users.length) state.users = data.users;
                if (data.bookings) state.bookings = data.bookings;
                if (data.packages && data.packages.length) state.packages = data.packages;
                if (data.coupons) state.coupons = data.coupons;
                saveStateToStorage();
                renderAllViews();
                updateUserUI();
            }
        } catch (e) {
            console.error("[DB Bootstrap Error]", e);
        }
    },

    async registerUser(userData) {
        if (this.isServerActive) {
            try {
                const res = await fetch("/api/users/register", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(userData)
                });
                return await res.json();
            } catch (e) {
                console.error("[DB Register Error]", e);
            }
        }
        return null;
    },

    async loginUser(credentials) {
        if (this.isServerActive) {
            try {
                const res = await fetch("/api/users/login", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(credentials)
                });
                return await res.json();
            } catch (e) {
                console.error("[DB Login Error]", e);
            }
        }
        return null;
    },

    async createBooking(bookingData) {
        if (this.isServerActive) {
            try {
                const res = await fetch("/api/bookings/create", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(bookingData)
                });
                return await res.json();
            } catch (e) {
                console.error("[DB Booking Error]", e);
            }
        }
        return null;
    },

    async cancelBooking(bookingId) {
        if (this.isServerActive) {
            try {
                const res = await fetch("/api/bookings/cancel", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ bookingId })
                });
                return await res.json();
            } catch (e) {
                console.error("[DB Cancel Error]", e);
            }
        }
        return null;
    },

    async updateWallet(username, amount) {
        if (this.isServerActive) {
            try {
                const res = await fetch("/api/users/update_wallet", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ username, amount })
                });
                return await res.json();
            } catch (e) {
                console.error("[DB Wallet Error]", e);
            }
        }
        return null;
    },

    async redeemPoints(username, points) {
        if (this.isServerActive) {
            try {
                const res = await fetch("/api/users/redeem_points", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ username, points })
                });
                return await res.json();
            } catch (e) {
                console.error("[DB Redeem Error]", e);
            }
        }
        return null;
    },

    async createPackage(pkgData) {
        if (this.isServerActive) {
            try {
                await fetch("/api/packages/create", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(pkgData)
                });
            } catch (e) {
                console.error("[DB Package Error]", e);
            }
        }
    },

    async createConnection(connData) {
        if (this.isServerActive) {
            try {
                await fetch("/api/connections/create", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(connData)
                });
            } catch (e) {
                console.error("[DB Connection Error]", e);
            }
        }
    }
};

// ============================================================================
// 1. INITIALIZATION & STORAGE
// ============================================================================
document.addEventListener("DOMContentLoaded", async () => {
    loadStateFromStorage();
    initGraph();
    renderAllViews();
    initSvgMap();
    initHeroSlideshow();
    checkExistingSession();
    await dbSync.init();
});

// ============================================================================
// HERO BACKGROUND SLIDESHOW CONTROLLER
// ============================================================================
let currentHeroSlide = 0;
let heroSlideTimer = null;
const totalHeroSlides = 4;

function initHeroSlideshow() {
    startHeroAutoSlide();
}

function startHeroAutoSlide() {
    if (heroSlideTimer) clearInterval(heroSlideTimer);
    heroSlideTimer = setInterval(() => {
        const next = (currentHeroSlide + 1) % totalHeroSlides;
        switchHeroSlide(next);
    }, 5000);
}

function switchHeroSlide(index) {
    currentHeroSlide = index;
    for (let i = 0; i < totalHeroSlides; i++) {
        const slide = document.getElementById(`heroSlide${i}`);
        if (slide) {
            if (i === index) slide.classList.add("active");
            else slide.classList.remove("active");
        }
        const thumb = document.getElementById(`heroThumb${i}`);
        if (thumb) {
            if (i === index) {
                thumb.className = "dest-card active-thumb p-3 rounded-2xl glass-panel-dark border-2 border-emerald-400 cursor-pointer overflow-hidden relative group shadow-lg shadow-emerald-500/20";
            } else {
                thumb.className = "dest-card p-3 rounded-2xl glass-panel-dark border border-slate-700/80 cursor-pointer overflow-hidden relative group opacity-75 hover:opacity-100 transition";
            }
        }
    }
}

function loadStateFromStorage() {
    const savedPackages = localStorage.getItem("tms_packages");
    const savedUsers = localStorage.getItem("tms_users");
    const savedBookings = localStorage.getItem("tms_bookings");
    const savedCoupons = localStorage.getItem("tms_coupons");
    const savedConnections = localStorage.getItem("tms_connections");

    state.packages = savedPackages ? JSON.parse(savedPackages) : [...DEFAULT_DATA.packages];
    state.users = savedUsers ? JSON.parse(savedUsers) : [...DEFAULT_DATA.users];
    state.bookings = savedBookings ? JSON.parse(savedBookings) : [];
    state.coupons = savedCoupons ? JSON.parse(savedCoupons) : { ...DEFAULT_DATA.coupons };
    state.connections = savedConnections ? JSON.parse(savedConnections) : [...DEFAULT_DATA.connections];
}

function saveStateToStorage() {
    localStorage.setItem("tms_packages", JSON.stringify(state.packages));
    localStorage.setItem("tms_users", JSON.stringify(state.users));
    localStorage.setItem("tms_bookings", JSON.stringify(state.bookings));
    localStorage.setItem("tms_coupons", JSON.stringify(state.coupons));
    localStorage.setItem("tms_connections", JSON.stringify(state.connections));
}

function initGraph() {
    state.graph = new GraphNetwork();
    DEFAULT_DATA.cities.forEach(c => state.graph.addCity(c.name, c.x, c.y, c.region, c.image, c.tagline, c.attractions));
    state.connections.forEach(c => state.graph.addConnection(c.from, c.to, c.distance));
}

function updateDestinationSpotlightAndBackdrop(destCityName) {
    if (!destCityName) return;
    const city = DEFAULT_DATA.cities.find(c => c.name.toLowerCase() === destCityName.toLowerCase());
    if (!city) return;

    // 1. Smoothly update background backdrop image of the Graph / Route Planner section
    const bgElem = document.getElementById("plannerBgImage");
    if (bgElem) {
        bgElem.style.backgroundImage = `url('${city.image}')`;
        bgElem.style.opacity = "0.35";
    }

    // 2. Update Tourist Spotlight Card
    const spotlightCard = document.getElementById("destTouristSpotlightCard");
    const imgElem = document.getElementById("spotlightCityImage");
    const nameElem = document.getElementById("spotlightCityName");
    const regElem = document.getElementById("spotlightRegion");
    const tagElem = document.getElementById("spotlightTagline");
    const pillsElem = document.getElementById("spotlightAttractionsPills");

    if (spotlightCard) spotlightCard.classList.remove("hidden");
    if (imgElem) imgElem.src = city.image;
    if (nameElem) nameElem.textContent = city.name;
    if (regElem) regElem.textContent = `${city.region} Division`;
    if (tagElem) tagElem.textContent = city.tagline;

    if (pillsElem && city.attractions) {
        pillsElem.innerHTML = city.attractions.map(attr => `
            <span class="text-[10px] bg-slate-900/90 text-emerald-300 border border-emerald-500/30 px-2 py-0.5 rounded-md font-semibold inline-flex items-center gap-1">
                <i class="fa-solid fa-location-dot text-[9px] text-rose-400"></i> ${attr}
            </span>
        `).join("");
    }
}

function checkExistingSession() {
    const savedUser = sessionStorage.getItem("tms_session_user");
    if (savedUser) {
        state.currentUser = JSON.parse(savedUser);
        updateUserUI();
    }
}

// ============================================================================
// 2. ROUTING & VIEW CONTROLLER
// ============================================================================
function showView(viewName) {
    document.querySelectorAll(".view-section").forEach(sec => sec.classList.add("hidden"));
    window.scrollTo({ top: 0, behavior: "smooth" });

    if (viewName === "home") {
        document.getElementById("viewHome").classList.remove("hidden");
    } else if (viewName === "planner") {
        document.getElementById("viewPlanner").classList.remove("hidden");
        populateCityDropdowns();
        updateDestinationSpotlightAndBackdrop(state.selectedDestination || "Cox's Bazar");
        drawSvgMap();
    } else if (viewName === "packages") {
        document.getElementById("viewPackages").classList.remove("hidden");
        renderPackagesCatalog();
    } else if (viewName === "customerDashboard") {
        if (!state.currentUser) {
            openAuthModal("login");
            return;
        }
        document.getElementById("viewCustomerDashboard").classList.remove("hidden");
        renderCustomerDashboard();
    } else if (viewName === "adminDashboard") {
        if (!state.currentUser || state.currentUser.role !== "ADMIN") {
            alert("Administrator authentication required!");
            openAuthModal("login");
            return;
        }
        document.getElementById("viewAdminDashboard").classList.remove("hidden");
        renderAdminDashboard();
    }
}

function renderAllViews() {
    renderHomePackages();
    renderPackagesCatalog();
    populateCityDropdowns();
}

// ============================================================================
// 3. SVG HIGHWAY MAP & DIJKSTRA VISUALIZATION
// ============================================================================
function initSvgMap() {
    drawSvgMap();
}

function drawSvgMap() {
    const edgesGroup = document.getElementById("svgEdgesLayer");
    const activePathGroup = document.getElementById("svgActivePathLayer");
    const nodesGroup = document.getElementById("svgNodesLayer");
    if (!edgesGroup || !nodesGroup) return;

    edgesGroup.innerHTML = "";
    activePathGroup.innerHTML = "";
    nodesGroup.innerHTML = "";

    const cityNodes = state.graph.getCities();
    const cityMap = new Map(cityNodes.map(c => [c.name, c]));

    // 1. Draw Highway Connections (Ocean Navy Road Lines)
    const connections = state.graph.getConnections();
    connections.forEach(conn => {
        const u = cityMap.get(conn.from);
        const v = cityMap.get(conn.to);
        if (u && v) {
            const line = document.createElementNS("http://www.w3.org/2000/svg", "line");
            line.setAttribute("x1", u.x);
            line.setAttribute("y1", u.y);
            line.setAttribute("x2", v.x);
            line.setAttribute("y2", v.y);
            line.setAttribute("stroke", "#1F325E");
            line.setAttribute("stroke-width", "2");
            line.setAttribute("stroke-opacity", "0.8");
            edgesGroup.appendChild(line);
        }
    });

    // 2. Draw Active Dijkstra Route Path (Glowing Sky Cyan & Gold Polyline)
    if (state.activeDijkstraResult && state.activeDijkstraResult.hasPath) {
        const path = state.activeDijkstraResult.path;
        for (let i = 0; i < path.length - 1; i++) {
            const u = cityMap.get(path[i]);
            const v = cityMap.get(path[i + 1]);
            if (u && v) {
                const glowLine = document.createElementNS("http://www.w3.org/2000/svg", "line");
                glowLine.setAttribute("x1", u.x);
                glowLine.setAttribute("y1", u.y);
                glowLine.setAttribute("x2", v.x);
                glowLine.setAttribute("y2", v.y);
                glowLine.setAttribute("stroke", "#06B6D4");
                glowLine.setAttribute("stroke-width", "5");
                glowLine.setAttribute("class", "route-animated");
                glowLine.setAttribute("stroke-linecap", "round");
                activePathGroup.appendChild(glowLine);
            }
        }
    }

    // 3. Draw City Nodes
    cityNodes.forEach(city => {
        const g = document.createElementNS("http://www.w3.org/2000/svg", "g");
        g.setAttribute("class", "cursor-pointer group");
        g.onclick = () => onNodeClicked(city.name);

        const isOrigin = state.selectedOrigin === city.name;
        const isTarget = state.selectedDestination === city.name;
        const inPath = state.activeDijkstraResult && state.activeDijkstraResult.path && state.activeDijkstraResult.path.includes(city.name);

        let fillColor = "#0284C7"; // Ocean Blue default
        let radius = 6;
        if (isOrigin) {
            fillColor = "#F27405"; // Sunset Orange Origin
            radius = 9;
        } else if (isTarget) {
            fillColor = "#F2B705"; // Sunshine Gold Target
            radius = 9;
        } else if (inPath) {
            fillColor = "#06B6D4"; // Sky Cyan Path
            radius = 7;
        }

        // Pulse ring for origin/target
        if (isOrigin || isTarget) {
            const pulse = document.createElementNS("http://www.w3.org/2000/svg", "circle");
            pulse.setAttribute("cx", city.x);
            pulse.setAttribute("cy", city.y);
            pulse.setAttribute("r", radius + 6);
            pulse.setAttribute("fill", fillColor);
            pulse.setAttribute("fill-opacity", "0.35");
            pulse.setAttribute("class", "node-pulse");
            g.appendChild(pulse);
        }

        const circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
        circle.setAttribute("cx", city.x);
        circle.setAttribute("cy", city.y);
        circle.setAttribute("r", radius);
        circle.setAttribute("fill", fillColor);
        circle.setAttribute("stroke", "#ffffff");
        circle.setAttribute("stroke-width", "1.5");
        g.appendChild(circle);

        // Text Label
        const text = document.createElementNS("http://www.w3.org/2000/svg", "text");
        text.setAttribute("x", city.x);
        text.setAttribute("y", city.y - 10);
        text.setAttribute("text-anchor", "middle");
        text.setAttribute("fill", isOrigin || isTarget || inPath ? "#FFFFFF" : "#94A3B8");
        text.setAttribute("font-size", isOrigin || isTarget ? "13" : "11");
        text.setAttribute("font-weight", isOrigin || isTarget ? "bold" : "600");
        text.textContent = city.name;
        g.appendChild(text);

        nodesGroup.appendChild(g);
    });
}

function onNodeClicked(cityName) {
    const originSelect = document.getElementById("selectOrigin");
    const destSelect = document.getElementById("selectDestination");

    if (!state.selectedOrigin || (state.selectedOrigin && state.selectedDestination)) {
        state.selectedOrigin = cityName;
        state.selectedDestination = null;
        if (originSelect) originSelect.value = cityName;
    } else if (state.selectedOrigin && !state.selectedDestination) {
        if (state.selectedOrigin === cityName) return;
        state.selectedDestination = cityName;
        if (destSelect) destSelect.value = cityName;
        updateDestinationSpotlightAndBackdrop(cityName);
        calculateDijkstraRoute();
    }
    drawSvgMap();
}

function populateCityDropdowns() {
    const originSelect = document.getElementById("selectOrigin");
    const destSelect = document.getElementById("selectDestination");
    if (!originSelect || !destSelect) return;

    const cities = state.graph.getCities().sort((a, b) => a.name.localeCompare(b.name));
    
    let html = `<option value="">-- Select City --</option>`;
    cities.forEach(c => {
        html += `<option value="${c.name}">${c.name} (${c.region})</option>`;
    });

    originSelect.innerHTML = html;
    destSelect.innerHTML = html;

    if (cities.length >= 2) {
        originSelect.value = state.selectedOrigin || "Dhaka";
        destSelect.value = state.selectedDestination || "Cox's Bazar";
        state.selectedOrigin = originSelect.value;
        state.selectedDestination = destSelect.value;
        updateDestinationSpotlightAndBackdrop(state.selectedDestination);
    }
}

function onCitySelectionChange() {
    state.selectedOrigin = document.getElementById("selectOrigin").value;
    state.selectedDestination = document.getElementById("selectDestination").value;
    if (state.selectedDestination) {
        updateDestinationSpotlightAndBackdrop(state.selectedDestination);
    }
    drawSvgMap();
}

function calculateDijkstraRoute() {
    const src = document.getElementById("selectOrigin").value;
    const dest = document.getElementById("selectDestination").value;

    if (!src || !dest) {
        alert("Please select both origin and destination cities!");
        return;
    }
    if (src === dest) {
        alert("Origin and Destination cannot be the same city!");
        return;
    }

    state.selectedOrigin = src;
    state.selectedDestination = dest;
    updateDestinationSpotlightAndBackdrop(dest);

    const res = state.graph.findShortestPath(src, dest);
    state.activeDijkstraResult = res;

    if (!res.hasPath) {
        alert(res.error || "No route found!");
        return;
    }

    // Render results in UI
    const card = document.getElementById("dijkstraResultCard");
    card.classList.remove("hidden");

    document.getElementById("resRouteTitle").textContent = `${src} to ${dest}`;
    document.getElementById("resTotalDistance").textContent = `${res.totalDistance} KM`;

    // Path sequence
    const seqContainer = document.getElementById("resPathSequence");
    seqContainer.innerHTML = res.path.map((node, idx) => `
        <span class="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-black ${idx === 0 ? 'btn-sunset-primary text-white' : (idx === res.path.length - 1 ? 'bg-sunset-coral text-white' : 'bg-warm-950 text-gold-300 border border-warm-700')}">
            ${idx > 0 ? '<i class="fa-solid fa-arrow-right text-[10px] opacity-70"></i>' : ''} ${node}
        </span>
    `).join("");

    // Calculate transport options
    const transportOptions = state.graph.calculateTransportOptions(res.totalDistance, DEFAULT_DATA.transportModes);
    const transportContainer = document.getElementById("resTransportOptions");
    transportContainer.innerHTML = transportOptions.map(t => `
        <div class="p-3.5 rounded-2xl bg-warm-950/95 border border-warm-800 hover:border-sunset-orange/60 transition flex items-center justify-between shadow-lg">
            <div class="flex items-center gap-3">
                <div class="w-10 h-10 rounded-xl bg-warm-900 border border-sunset-orange/30 flex items-center justify-center text-sunset-orange">
                    <i class="fa-solid ${t.icon}"></i>
                </div>
                <div>
                    <div class="flex items-center gap-2">
                        <span class="text-sm font-bold text-white">${t.name}</span>
                        <span class="text-[10px] font-bold badge-gold px-2 py-0.5 rounded-md">${t.badge}</span>
                    </div>
                    <span class="text-xs text-warm-300">Est. Time: <strong class="text-white">${t.durationString}</strong> (${t.speed} km/h)</span>
                </div>
            </div>
            <div class="text-right">
                <div class="text-base font-black text-gold-400">${t.fare.toLocaleString()} BDT</div>
                <button onclick="startCustomTripCheckout('${src}', '${dest}', ${res.totalDistance}, '${t.name}', ${t.fare})" class="mt-1 px-3.5 py-1.5 btn-sunset-primary text-xs font-black rounded-xl transition shadow-md">
                    Book Trip
                </button>
            </div>
        </div>
    `).join("");

    drawSvgMap();
}

// ============================================================================
// 4. PACKAGES CATALOG & RENDERING
// ============================================================================
function renderHomePackages() {
    const grid = document.getElementById("homePackagesGrid");
    if (!grid) return;
    grid.innerHTML = state.packages.slice(0, 3).map(pkg => createPackageCardHtml(pkg)).join("");
}

function renderPackagesCatalog(filter = "all") {
    const grid = document.getElementById("catalogPackagesGrid");
    if (!grid) return;

    let filtered = state.packages;
    if (filter !== "all") {
        filtered = state.packages.filter(p => 
            p.destination.toLowerCase().includes(filter.toLowerCase()) || 
            p.name.toLowerCase().includes(filter.toLowerCase())
        );
    }
    grid.innerHTML = filtered.map(pkg => createPackageCardHtml(pkg)).join("");
}

function filterPackages(category) {
    document.querySelectorAll(".pkg-filter-btn").forEach(btn => {
        btn.className = "pkg-filter-btn px-4 py-1.5 rounded-full text-xs font-bold bg-warm-900 text-warm-200 hover:text-white border border-warm-700 transition";
    });
    event.target.className = "pkg-filter-btn px-4 py-1.5 rounded-full text-xs font-black btn-sunset-primary transition";
    renderPackagesCatalog(category);
}

function createPackageCardHtml(pkg) {
    return `
        <div class="glass-panel-dark rounded-3xl overflow-hidden border border-warm-700/80 hover:border-sunset-orange/70 transition-all duration-300 transform hover:-translate-y-1 shadow-2xl flex flex-col group">
            <div class="relative h-52 overflow-hidden">
                <img src="${pkg.image}" alt="${pkg.name}" class="w-full h-full object-cover group-hover:scale-110 transition-transform duration-700">
                <div class="absolute top-3 right-3 bg-warm-950/90 backdrop-blur-md px-3 py-1 rounded-full text-xs font-black text-gold-400 border border-warm-700 shadow-md">
                    ${pkg.durationDays}D / ${pkg.durationNights}N
                </div>
                <div class="absolute bottom-3 left-3 bg-warm-950/90 backdrop-blur-md px-2.5 py-1 rounded-lg text-xs font-bold text-white shadow-md">
                    <i class="fa-solid fa-location-dot text-sunset-coral mr-1"></i>${pkg.destination}
                </div>
            </div>
            <div class="p-6 flex-grow flex flex-col justify-between space-y-4">
                <div>
                    <h3 class="text-lg font-black text-white leading-snug">${pkg.name}</h3>
                    <p class="text-xs text-warm-200 mt-2 line-clamp-2">${pkg.description}</p>
                    <div class="mt-3 flex items-center gap-1.5 text-xs text-gold-400 font-bold">
                        <i class="fa-solid fa-hotel"></i> ${pkg.hotelRating}
                    </div>
                </div>

                <div class="pt-4 border-t border-warm-800 flex items-center justify-between">
                    <div>
                        <span class="text-[11px] text-warm-400 block font-semibold">Starting from</span>
                        <span class="text-xl font-black text-gold-400">${pkg.cost.toLocaleString()} BDT</span>
                    </div>
                    <button onclick="startPackageCheckout('${pkg.id}')" class="px-5 py-2.5 btn-sunset-primary font-black text-xs rounded-xl transition shadow-lg shadow-sunset-orange/20">
                        Book Holiday
                    </button>
                </div>
            </div>
        </div>
    `;
}

// ============================================================================
// 5. BOOKING ENGINE & CHECKOUT MODAL
// ============================================================================
function startPackageCheckout(packageId) {
    if (!state.currentUser) {
        alert("Please log in or register to complete your booking!");
        openAuthModal("login");
        return;
    }
    const pkg = state.packages.find(p => p.id === packageId);
    if (!pkg) return;

    state.activeCheckout = {
        type: "PACKAGE",
        title: pkg.name,
        destination: pkg.destination,
        transport: "Luxury AC Tourist Coach",
        baseCost: pkg.cost,
        appliedCoupon: null,
        couponDiscount: 0
    };

    openCheckoutModal();
}

function startCustomTripCheckout(src, dest, distance, transportName, fare) {
    if (!state.currentUser) {
        alert("Please log in or register to complete your booking!");
        openAuthModal("login");
        return;
    }

    state.activeCheckout = {
        type: "CUSTOM",
        title: `${src} to ${dest}`,
        destination: dest,
        source: src,
        distanceKm: distance,
        transport: transportName,
        baseCost: fare,
        appliedCoupon: null,
        couponDiscount: 0
    };

    openCheckoutModal();
}

function openCheckoutModal() {
    updateCheckoutCalculations();
    document.getElementById("modalCheckout").classList.remove("hidden");
}

function updateCheckoutCalculations() {
    const chk = state.activeCheckout;
    if (!chk) return;

    const user = state.currentUser;
    let tierDiscountPercent = 0;
    let tierName = "Silver";
    if (user && user.tier === "PLATINUM") { tierDiscountPercent = 0.10; tierName = "Platinum 10%"; }
    else if (user && user.tier === "GOLD") { tierDiscountPercent = 0.05; tierName = "Gold 5%"; }

    const tierDiscount = chk.baseCost * tierDiscountPercent;
    const subtotalAfterDiscounts = Math.max(0, chk.baseCost - tierDiscount - chk.couponDiscount);
    const vat = subtotalAfterDiscounts * 0.05;
    const finalAmount = subtotalAfterDiscounts + vat;

    chk.finalCost = finalAmount;
    chk.tierDiscount = tierDiscount;
    chk.vat = vat;

    document.getElementById("chkTripTitle").textContent = chk.title;
    document.getElementById("chkTransportName").textContent = chk.transport;
    document.getElementById("chkBasePrice").textContent = `${chk.baseCost.toLocaleString()} BDT`;
    document.getElementById("chkTierName").textContent = tierName;
    document.getElementById("chkTierDiscount").textContent = `-${tierDiscount.toFixed(2)} BDT`;
    document.getElementById("chkCouponDiscount").textContent = `-${chk.couponDiscount.toFixed(2)} BDT`;
    document.getElementById("chkVatAmount").textContent = `${vat.toFixed(2)} BDT`;
    document.getElementById("chkFinalAmount").textContent = `${finalAmount.toLocaleString(undefined, {minimumFractionDigits: 2, maximumFractionDigits: 2})} BDT`;
}

function applyCheckoutCoupon() {
    const code = document.getElementById("chkCouponInput").value.trim().toUpperCase();
    if (!code) return;

    if (state.coupons[code] !== undefined) {
        const val = state.coupons[code];
        let disc = 0;
        if (val < 1.0) {
            disc = state.activeCheckout.baseCost * val;
        } else {
            disc = Math.min(val, state.activeCheckout.baseCost * 0.8);
        }
        state.activeCheckout.appliedCoupon = code;
        state.activeCheckout.couponDiscount = disc;
        updateCheckoutCalculations();
        alert(`Promo Coupon "${code}" Applied Successfully! Saved ${disc.toFixed(2)} BDT.`);
    } else {
        alert("Invalid or expired promo coupon code.");
    }
}

function executePaymentFlow() {
    const strategy = document.querySelector('input[name="payStrategy"]:checked').value;
    const chk = state.activeCheckout;

    if (strategy === "WALLET") {
        if (state.currentUser.walletBalance < chk.finalCost) {
            alert(`Insufficient funds in Travel Wallet! Current Balance: ${state.currentUser.walletBalance.toLocaleString()} BDT.`);
            return;
        }
        state.currentUser.walletBalance -= chk.finalCost;
        completeBooking("Internal App Wallet", `WAL-TXN-${Math.floor(100000 + Math.random() * 900000)}`);
    } else if (strategy === "BKASH" || strategy === "NAGAD") {
        closeModal("modalCheckout");
        openGatewayModal(strategy, chk.finalCost);
    } else if (strategy === "CARD") {
        closeModal("modalCheckout");
        openGatewayModal("CARD", chk.finalCost);
    }
}

// ============================================================================
// 6. PAYMENT GATEWAYS SIMULATION (STRATEGY PATTERN)
// ============================================================================
function openGatewayModal(provider, amount) {
    const modal = document.getElementById("modalGateway");
    const header = document.getElementById("gatewayHeader");
    const title = document.getElementById("gatewayTitle");
    const amt = document.getElementById("gatewayAmount");

    amt.textContent = `${amount.toFixed(2)} BDT`;

    if (provider === "BKASH") {
        header.className = "bkash-header p-6 text-white text-center relative";
        title.textContent = "bKash Payment";
    } else if (provider === "NAGAD") {
        header.className = "nagad-header p-6 text-white text-center relative";
        title.textContent = "Nagad Payment";
    } else {
        header.className = "bg-slate-800 p-6 text-white text-center relative";
        title.textContent = "Credit/Debit Card Checkout";
    }

    modal.classList.remove("hidden");
}

function confirmGatewayPayment() {
    const phone = document.getElementById("gatewayPhone").value.trim();
    const pin = document.getElementById("gatewayPin").value.trim();

    if (!phone || !pin) {
        alert("Please enter account phone and security PIN!");
        return;
    }

    closeModal("modalGateway");
    const txId = `TXN-${Date.now().toString().slice(-8)}`;
    completeBooking("Digital Payment Gateway", txId);
}

async function completeBooking(paymentMethod, txId) {
    const chk = state.activeCheckout;
    const user = state.currentUser;
    const now = new Date().toISOString().replace('T', ' ').substring(0, 19);
    const bookingId = `BK-${new Date().getFullYear()}-${Math.floor(1000 + Math.random() * 9000)}`;

    const newBooking = {
        bookingId: bookingId,
        username: user.username,
        customerName: user.fullName,
        customerPhone: user.phone,
        tripType: chk.type === "PACKAGE" ? "Tour Package" : "Custom Route",
        tripTitle: chk.title,
        source: chk.source || "Dhaka (HQ)",
        destination: chk.destination,
        routePath: chk.source ? `${chk.source} ===> ${chk.destination}` : `Direct Itinerary to ${chk.destination}`,
        transportName: chk.transport,
        distanceKm: chk.distanceKm || 0,
        bookingDate: now,
        subtotal: chk.baseCost,
        discountAmount: chk.tierDiscount + chk.couponDiscount,
        couponCode: chk.appliedCoupon || "",
        vatAmount: chk.vat,
        finalCost: chk.finalCost,
        status: "CONFIRMED",
        paymentMethod: paymentMethod,
        transactionId: txId
    };

    state.bookings.unshift(newBooking);
    user.bookingHistory.push(bookingId);
    user.loyaltyPoints += Math.floor(chk.finalCost / 100);

    // Save to SQLite Database
    await dbSync.createBooking(newBooking);

    // Save to user storage
    const uIdx = state.users.findIndex(u => u.username === user.username);
    if (uIdx !== -1) state.users[uIdx] = user;

    saveStateToStorage();
    sessionStorage.setItem("tms_session_user", JSON.stringify(user));

    closeModal("modalCheckout");
    updateUserUI();
    showTicketModal(newBooking);
}

// ============================================================================
// 7. BOARDING PASS / TICKET MODAL
// ============================================================================
function showTicketModal(b) {
    document.getElementById("tktBookingId").textContent = b.bookingId;
    document.getElementById("tktPassengerName").textContent = b.customerName;
    document.getElementById("tktPhone").textContent = b.customerPhone;
    document.getElementById("tktTripTitle").textContent = b.tripTitle;
    document.getElementById("tktRoutePath").textContent = b.routePath;
    document.getElementById("tktTransport").textContent = b.transportName;
    document.getElementById("tktDate").textContent = b.bookingDate;
    document.getElementById("tktTotalFare").textContent = `${b.finalCost.toFixed(2)} BDT`;
    document.getElementById("tktPaymentMethod").textContent = `Paid via ${b.paymentMethod} (${b.transactionId})`;

    document.getElementById("modalTicket").classList.remove("hidden");
}

// ============================================================================
// 8. CUSTOMER & ADMIN DASHBOARDS
// ============================================================================
function renderCustomerDashboard() {
    const user = state.currentUser;
    if (!user) return;

    document.getElementById("custDashboardGreeting").textContent = `Welcome back, ${user.fullName}`;
    
    const avatar = document.getElementById("custProfileAvatar");
    if (avatar) avatar.textContent = user.fullName ? user.fullName.charAt(0).toUpperCase() : "U";

    const tierBadge = document.getElementById("custHeaderTierBadge");
    if (tierBadge) {
        tierBadge.innerHTML = `<i class="fa-solid fa-crown mr-1 text-amber-400"></i>${user.tier || "Silver"} Member`;
    }

    document.getElementById("custStatWallet").textContent = `${user.walletBalance.toLocaleString()} BDT`;
    document.getElementById("custStatPoints").textContent = `${user.loyaltyPoints} pts`;
    document.getElementById("custStatTier").textContent = `${user.tier || "Silver"}`;

    const myBookings = state.bookings.filter(b => b.username === user.username);
    document.getElementById("custStatTrips").textContent = myBookings.length;

    const tbody = document.getElementById("custBookingsTableBody");
    if (myBookings.length === 0) {
        tbody.innerHTML = `<tr><td colspan="7" class="px-6 py-8 text-center text-warm-400 font-semibold">No bookings yet. Explore packages or plan custom journeys above!</td></tr>`;
        return;
    }

    tbody.innerHTML = myBookings.map(b => `
        <tr class="hover:bg-ocean-900/60 transition">
            <td class="px-6 py-4 font-mono font-black text-sky-light">${b.bookingId}</td>
            <td class="px-6 py-4 font-bold text-white">${b.tripTitle}</td>
            <td class="px-6 py-4 text-xs text-slate-300 font-medium">${b.transportName}</td>
            <td class="px-6 py-4 font-black text-gold-400">${b.finalCost.toLocaleString()} BDT</td>
            <td class="px-6 py-4">
                <span class="px-3 py-1 rounded-full text-xs font-black ${b.status === 'CONFIRMED' ? 'badge-eco' : 'badge-coral'}">
                    ${b.status}
                </span>
            </td>
            <td class="px-6 py-4 text-xs text-slate-400">${b.bookingDate.split(' ')[0]}</td>
            <td class="px-6 py-4 text-right space-x-2">
                <button onclick='showTicketModal(${JSON.stringify(b)})' class="text-xs bg-ocean-900 hover:bg-ocean-800 text-sky-light px-3.5 py-1.5 rounded-xl border border-ocean-700 font-bold transition">
                    <i class="fa-solid fa-ticket mr-1"></i> Ticket
                </button>
                ${b.status === 'CONFIRMED' ? `
                    <button onclick="cancelBooking('${b.bookingId}')" class="text-xs bg-rose-500/15 hover:bg-rose-500/25 text-rose-400 px-3 py-1.5 rounded-xl border border-rose-500/30 font-bold transition">
                        Cancel (90% Refund)
                    </button>
                ` : ''}
            </td>
        </tr>
    `).join("");
}

async function cancelBooking(bookingId) {
    if (!confirm("Are you sure you want to cancel this booking? 90% of fare will be refunded to your Travel Wallet.")) return;

    const b = state.bookings.find(item => item.bookingId === bookingId);
    if (!b) return;

    b.status = "CANCELLED";
    const refund = b.finalCost * 0.90;
    state.currentUser.walletBalance += refund;

    // Save cancellation to SQLite DB
    await dbSync.cancelBooking(bookingId);

    const uIdx = state.users.findIndex(u => u.username === state.currentUser.username);
    if (uIdx !== -1) state.users[uIdx] = state.currentUser;

    saveStateToStorage();
    sessionStorage.setItem("tms_session_user", JSON.stringify(state.currentUser));

    updateUserUI();
    renderCustomerDashboard();
    alert(`Booking ${bookingId} Cancelled! ${refund.toFixed(2)} BDT has been credited to your Travel Wallet.`);
}

function renderAdminDashboard() {
    let revenue = 0, discounts = 0, confirmedCount = 0;
    const destPopularity = {};

    state.bookings.forEach(b => {
        if (b.status === "CONFIRMED") {
            revenue += b.finalCost;
            discounts += b.discountAmount;
            confirmedCount++;
            destPopularity[b.destination] = (destPopularity[b.destination] || 0) + 1;
        }
    });

    document.getElementById("admStatRevenue").textContent = `${revenue.toLocaleString()} BDT`;
    document.getElementById("admStatBookings").textContent = confirmedCount;
    document.getElementById("admStatUsers").textContent = state.users.filter(u => u.role === "CUSTOMER").length;
    document.getElementById("admStatDiscounts").textContent = `${discounts.toLocaleString()} BDT`;

    // Render Master Bookings
    const tbody = document.getElementById("admBookingsTableBody");
    tbody.innerHTML = state.bookings.map(b => `
        <tr class="hover:bg-ocean-900/60 transition">
            <td class="px-6 py-4 font-mono font-black text-sky-light">${b.bookingId}</td>
            <td class="px-6 py-4 font-bold text-white">${b.customerName} (${b.username})</td>
            <td class="px-6 py-4 text-xs text-slate-300">${b.tripTitle}</td>
            <td class="px-6 py-4 font-black text-gold-400">${b.finalCost.toLocaleString()} BDT</td>
            <td class="px-6 py-4">
                <span class="px-3 py-1 rounded-full text-xs font-black ${b.status === 'CONFIRMED' ? 'badge-eco' : 'badge-coral'}">
                    ${b.status}
                </span>
            </td>
            <td class="px-6 py-4 text-xs font-mono text-slate-400">${b.transactionId}</td>
            <td class="px-6 py-4 text-right">
                <button onclick='showTicketModal(${JSON.stringify(b)})' class="text-xs bg-ocean-900 text-sky-light px-3 py-1 rounded-lg border border-ocean-700 font-bold">View</button>
            </td>
        </tr>
    `).join("");

    // Render Coupons
    const couponContainer = document.getElementById("admCouponList");
    couponContainer.innerHTML = Object.entries(state.coupons).map(([k, v]) => `
        <div class="flex items-center justify-between p-2.5 bg-ocean-950/90 rounded-xl border border-ocean-800">
            <span class="font-mono font-black text-gold-400 text-xs">${k}</span>
            <span class="text-xs text-slate-300 font-semibold">${v < 1.0 ? `${v * 100}% Discount` : `${v} BDT Off`}</span>
        </div>
    `).join("");

    // Render Chart.js
    renderAdminChart(destPopularity);
}

let adminChartInstance = null;
function renderAdminChart(destMap) {
    const ctx = document.getElementById("chartPopularDestinations");
    if (!ctx) return;

    if (adminChartInstance) adminChartInstance.destroy();

    const labels = Object.keys(destMap).length > 0 ? Object.keys(destMap) : ["Cox's Bazar", "Sylhet", "Sundarbans", "Rajshahi", "Barisal"];
    const values = Object.keys(destMap).length > 0 ? Object.values(destMap) : [12, 9, 6, 4, 3];

    adminChartInstance = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: labels,
            datasets: [{
                label: 'Trips Booked',
                data: values,
                backgroundColor: 'rgba(2, 132, 199, 0.75)',
                borderColor: '#38BDF8',
                borderWidth: 2,
                borderRadius: 10
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { display: false }
            },
            scales: {
                y: { beginAtZero: true, grid: { color: 'rgba(56, 189, 248, 0.1)' }, ticks: { color: '#94A3B8' } },
                x: { grid: { display: false }, ticks: { color: '#94A3B8' } }
            }
        }
    });
}

async function handleAdminAddConnection() {
    const from = document.getElementById("admNewFrom").value.trim();
    const to = document.getElementById("admNewTo").value.trim();
    const dist = parseInt(document.getElementById("admNewDist").value);

    if (!from || !to || isNaN(dist) || dist <= 0) {
        alert("Please enter valid cities and positive distance!");
        return;
    }

    state.graph.addConnection(from, to, dist);
    state.connections.push({ from, to, distance: dist });
    await dbSync.createConnection({ from, to, distance: dist });

    saveStateToStorage();
    alert(`Highway connection between ${from} and ${to} (${dist} KM) added to SQLite Database & Graph!`);
    drawSvgMap();
}

async function handleAdminAddPackage() {
    const name = document.getElementById("admPkgName").value.trim();
    const dest = document.getElementById("admPkgDest").value.trim();
    const cost = parseFloat(document.getElementById("admPkgCost").value);
    const hotel = document.getElementById("admPkgHotel").value.trim();
    const inc = document.getElementById("admPkgInclusions").value.trim();

    if (!name || !dest || isNaN(cost) || cost <= 0) {
        alert("Please fill in all package details!");
        return;
    }

    const newPkg = {
        id: `PKG-${100 + state.packages.length + 1}`,
        name: name,
        destination: dest,
        cost: cost,
        durationDays: 3,
        durationNights: 2,
        hotelRating: hotel || "★★★★☆ Luxury Resort",
        inclusions: inc || "AC Transport, Hotel, Breakfast",
        description: `Explore the sights and scenic attractions of ${dest}.`,
        image: "https://images.unsplash.com/photo-1596895111956-bf1cf0599ce5?w=800&q=80"
    };

    state.packages.push(newPkg);
    await dbSync.createPackage(newPkg);

    saveStateToStorage();
    renderAllViews();
    alert(`Tour Package "${name}" published and saved to SQLite Database!`);
}

// ============================================================================
// 9. AUTHENTICATION & WALLET MODALS
// ============================================================================
function openAuthModal(tab = "login") {
    switchAuthTab(tab);
    document.getElementById("modalAuth").classList.remove("hidden");
}

function switchAuthTab(tab) {
    const tabLogin = document.getElementById("tabAuthLogin");
    const tabReg = document.getElementById("tabAuthRegister");
    const formLogin = document.getElementById("formLogin");
    const formReg = document.getElementById("formRegister");

    if (tab === "login") {
        tabLogin.className = "w-1/2 py-2 text-sm font-black rounded-xl btn-sunset text-white transition shadow-md";
        tabReg.className = "w-1/2 py-2 text-sm font-bold text-slate-300 hover:text-white transition";
        formLogin.classList.remove("hidden");
        formReg.classList.add("hidden");
    } else {
        tabReg.className = "w-1/2 py-2 text-sm font-black rounded-xl btn-sunset text-white transition shadow-md";
        tabLogin.className = "w-1/2 py-2 text-sm font-bold text-slate-300 hover:text-white transition";
        formReg.classList.remove("hidden");
        formLogin.classList.add("hidden");
    }
}

async function handleLoginForm(e) {
    e.preventDefault();
    const uname = document.getElementById("loginUsername").value.trim();
    const pass = document.getElementById("loginPassword").value.trim();

    let user = state.users.find(u => u.username === uname);
    if (dbSync.isServerActive) {
        const res = await dbSync.loginUser({ username: uname, password: pass });
        if (res && res.success && res.user) {
            user = res.user;
        } else if (res && !res.success) {
            alert(res.error || "Authentication failed!");
            return;
        }
    }

    if (!user) {
        alert("Username not found!");
        return;
    }
    if (user.passwordHash && user.passwordHash !== pass) {
        alert("Incorrect password!");
        return;
    }

    state.currentUser = user;
    sessionStorage.setItem("tms_session_user", JSON.stringify(user));
    closeModal("modalAuth");
    updateUserUI();

    if (user.role === "ADMIN") {
        showView("adminDashboard");
    } else {
        showView("customerDashboard");
    }
}

async function handleRegisterForm(e) {
    e.preventDefault();
    const uname = document.getElementById("regUsername").value.trim();
    const pass = document.getElementById("regPassword").value.trim();
    const name = document.getElementById("regFullName").value.trim();
    const phone = document.getElementById("regPhone").value.trim();
    const email = document.getElementById("regEmail").value.trim();
    const addr = document.getElementById("regAddress").value.trim();

    if (state.users.some(u => u.username === uname)) {
        alert("Username already taken!");
        return;
    }

    const newUser = {
        username: uname,
        passwordHash: pass,
        password: pass,
        fullName: name,
        phone: phone,
        email: email,
        address: addr,
        role: "CUSTOMER",
        walletBalance: 500, // 500 BDT bonus
        loyaltyPoints: 50,
        tier: "SILVER",
        bookingHistory: []
    };

    // Save to SQLite Database
    await dbSync.registerUser(newUser);

    state.users.push(newUser);
    saveStateToStorage();

    state.currentUser = newUser;
    sessionStorage.setItem("tms_session_user", JSON.stringify(newUser));
    closeModal("modalAuth");
    updateUserUI();
    alert("Account created and saved to SQLite Database! 500 BDT welcome bonus credited to your wallet.");
    showView("customerDashboard");
}

function handleLogout() {
    state.currentUser = null;
    sessionStorage.removeItem("tms_session_user");
    updateUserUI();
    showView("home");
}

function updateUserUI() {
    const guestGroup = document.getElementById("guestAuthGroup");
    const profileGroup = document.getElementById("userProfileGroup");
    const walletBadge = document.getElementById("userWalletBadge");
    const custLink = document.getElementById("navCustomerLink");
    const adminLink = document.getElementById("navAdminLink");

    if (state.currentUser) {
        guestGroup.classList.add("hidden");
        profileGroup.classList.remove("hidden");
        document.getElementById("navUserName").textContent = state.currentUser.fullName.split(' ')[0];
        document.getElementById("dropdownUserFullname").textContent = state.currentUser.fullName;
        document.getElementById("userAvatarLetter").textContent = state.currentUser.fullName.charAt(0).toUpperCase();

        if (state.currentUser.role === "CUSTOMER") {
            walletBadge.classList.remove("hidden");
            walletBadge.classList.add("flex");
            document.getElementById("navWalletBalance").textContent = `${state.currentUser.walletBalance.toLocaleString()} BDT`;
            custLink.classList.remove("hidden");
            adminLink.classList.add("hidden");
        } else {
            walletBadge.classList.add("hidden");
            custLink.classList.add("hidden");
            adminLink.classList.remove("hidden");
        }
    } else {
        guestGroup.classList.remove("hidden");
        profileGroup.classList.add("hidden");
        walletBadge.classList.add("hidden");
        custLink.classList.add("hidden");
        adminLink.classList.add("hidden");
    }
}

function toggleUserDropdown() {
    const menu = document.getElementById("userDropdownMenu");
    menu.classList.toggle("hidden");
}

function openWalletTopUpModal() {
    if (!state.currentUser) { openAuthModal(); return; }
    document.getElementById("modalTopUp").classList.remove("hidden");
}

async function handleWalletTopUp() {
    const amt = parseFloat(document.getElementById("topUpAmount").value);
    if (isNaN(amt) || amt <= 0) {
        alert("Please enter a valid top-up amount!");
        return;
    }

    state.currentUser.walletBalance += amt;
    await dbSync.updateWallet(state.currentUser.username, amt);

    const uIdx = state.users.findIndex(u => u.username === state.currentUser.username);
    if (uIdx !== -1) state.users[uIdx] = state.currentUser;

    saveStateToStorage();
    sessionStorage.setItem("tms_session_user", JSON.stringify(state.currentUser));

    updateUserUI();
    closeModal("modalTopUp");
    alert(`Wallet Recharged in Database! New Balance: ${state.currentUser.walletBalance.toLocaleString()} BDT.`);
    if (!document.getElementById("viewCustomerDashboard").classList.contains("hidden")) {
        renderCustomerDashboard();
    }
}

function openRedeemPointsModal() {
    if (!state.currentUser) { openAuthModal(); return; }
    document.getElementById("redeemModalPoints").textContent = `${state.currentUser.loyaltyPoints} pts`;
    document.getElementById("modalRedeem").classList.remove("hidden");
}

async function handleRedeemPoints() {
    const pts = parseInt(document.getElementById("redeemPointsInput").value);
    if (isNaN(pts) || pts < 50 || pts > state.currentUser.loyaltyPoints) {
        alert("Please enter at least 50 points and not more than your available balance!");
        return;
    }

    const cash = pts * 0.5;
    state.currentUser.loyaltyPoints -= pts;
    state.currentUser.walletBalance += cash;

    await dbSync.redeemPoints(state.currentUser.username, pts);

    const uIdx = state.users.findIndex(u => u.username === state.currentUser.username);
    if (uIdx !== -1) state.users[uIdx] = state.currentUser;

    saveStateToStorage();
    sessionStorage.setItem("tms_session_user", JSON.stringify(state.currentUser));

    updateUserUI();
    closeModal("modalRedeem");
    alert(`Converted ${pts} reward points into ${cash.toFixed(2)} BDT Travel Wallet credit in Database!`);
    if (!document.getElementById("viewCustomerDashboard").classList.contains("hidden")) {
        renderCustomerDashboard();
    }
}

function closeModal(modalId) {
    document.getElementById(modalId).classList.add("hidden");
}
