/**
 * Bangladesh Tourism & Travel Management System - Seed Data, Coordinates & Tourist Destinations
 */

const DEFAULT_DATA = {
    // 21 Major Districts with Coordinates, High-Res Tourist Place Imagery, and Landmark Highlights
    cities: [
        {
            name: "Cox's Bazar",
            x: 680,
            y: 770,
            region: "Chittagong",
            image: "https://images.unsplash.com/photo-1596895111956-bf1cf0599ce5?w=1600&q=85",
            tagline: "World's Longest Natural Sea Beach (120 KM)",
            attractions: ["Inani Coral Beach", "Marine Drive", "Himchari Waterfalls", "Saint Martin Coral Island", "Moheshkhali Temple"]
        },
        {
            name: "Sylhet",
            x: 670,
            y: 260,
            region: "Sylhet",
            image: "https://images.unsplash.com/photo-1608958435020-e8a7109ba809?w=1600&q=85",
            tagline: "Land of Two Leaves and a Bud & Green Springs",
            attractions: ["Ratargul Swamp Forest", "Jaflong River Stones", "Bichnakandi Spring", "Sreemangal Tea Estates", "Shah Jalal Shrine"]
        },
        {
            name: "Chittagong",
            x: 630,
            y: 640,
            region: "Chittagong",
            image: "https://images.unsplash.com/photo-1582650625119-3a31f8418b7d?w=1600&q=85",
            tagline: "Commercial Capital & Scenic Coastal Haven",
            attractions: ["Patenga Sea Beach", "Foy's Lake", "Naval Beach", "Chandranath Hill & Temple", "Bhatiari Sunset Lake"]
        },
        {
            name: "Khulna",
            x: 300,
            y: 620,
            region: "Khulna",
            image: "https://images.unsplash.com/photo-1544735716-392fe2489ffa?w=1600&q=85",
            tagline: "Gateway to the World's Largest Mangrove Forest",
            attractions: ["Sundarbans UNESCO Forest", "Kotka Wildlife Sanctuary", "Karamjal Crocodile Center", "Hiron Point Watchtower"]
        },
        {
            name: "Rajshahi",
            x: 200,
            y: 350,
            region: "Rajshahi",
            image: "https://images.unsplash.com/photo-1548013146-72479768bada?w=1600&q=85",
            tagline: "Silk City & Ancient Terracotta Heritage",
            attractions: ["Puthia Terracotta Palace", "Padma Riverfront Sunset", "Varendra Research Museum", "Bagha Mosque"]
        },
        {
            name: "Barisal",
            x: 420,
            y: 640,
            region: "Barisal",
            image: "https://images.unsplash.com/photo-1507525428034-b723cf961d3e?w=1600&q=85",
            tagline: "Venice of Bengal & Floating River Markets",
            attractions: ["Floating Guava Market", "Kuakata Daughter of Sea", "Sandhya River Backwaters", "Durga Sagar Dighi"]
        },
        {
            name: "Rangpur",
            x: 280,
            y: 150,
            region: "Rangpur",
            image: "https://images.unsplash.com/photo-1564507592333-c60657eea523?w=1600&q=85",
            tagline: "Royal Palaces & Northern Cultural Heritage",
            attractions: ["Historic Tajhat Palace", "Carmichael Heritage Campus", "Chikli Water Park", "Kellaband Buddhist Stupa"]
        },
        {
            name: "Dinajpur",
            x: 170,
            y: 160,
            region: "Rangpur",
            image: "https://images.unsplash.com/photo-1584551246679-0daf3d275d0f?w=1600&q=85",
            tagline: "18th Century Terracotta Architecture & Lakes",
            attractions: ["Kantajew Terracotta Temple", "Ramsagar Mega Dighi", "Swapnapuri Amusement World", "Nayabad Mosque"]
        },
        {
            name: "Bogra",
            x: 310,
            y: 260,
            region: "Rajshahi",
            image: "https://images.unsplash.com/photo-1518684079-3c830dcef090?w=1600&q=85",
            tagline: "Archaeological Capital (3rd Century BC)",
            attractions: ["Mahasthangarh Citadel", "Vasu Vihara Monastic Ruins", "Kherua 16th Century Mosque", "Bogra Sweets Bazaar"]
        },
        {
            name: "Tangail",
            x: 380,
            y: 360,
            region: "Dhaka",
            image: "https://images.unsplash.com/photo-1506744038136-46273834b3fb?w=1600&q=85",
            tagline: "Zamindar Palaces & World-Famous Weaving",
            attractions: ["Mohera Zamindar Estate", "201 Dome Mosque", "Madhupur Sal National Forest", "Dhanbari Palace"]
        },
        {
            name: "Mymensingh",
            x: 470,
            y: 290,
            region: "Mymensingh",
            image: "https://images.unsplash.com/photo-1513836279014-a89f7a76ae86?w=1600&q=85",
            tagline: "Brahmaputra Riverside & Shashi Palace",
            attractions: ["Shashi Lodge (Crystal Palace)", "Alexander Castle", "Brahmaputra Riverfront Park", "Garo Foothills Ecopark"]
        },
        {
            name: "Jessore",
            x: 260,
            y: 550,
            region: "Khulna",
            image: "https://images.unsplash.com/photo-1469854523086-cc02fe5d8800?w=1600&q=85",
            tagline: "Flower Capital & Michael Madhusudan Memorial",
            attractions: ["Gadkhali Flower Gardens", "Sagardari Madhusudan Estate", "Chanchra Rajbari Palace", "Collectorate Park"]
        },
        {
            name: "Comilla",
            x: 550,
            y: 500,
            region: "Chittagong",
            image: "https://images.unsplash.com/photo-1509316975850-ff9c5deb0cd9?w=1600&q=85",
            tagline: "8th Century Buddhist Monasteries & Hills",
            attractions: ["Shalban Vihara Archaeological Site", "Mainamati War Cemetery", "Dharmasagar Dighi", "Kotbari Hills"]
        },
        {
            name: "Dhaka",
            x: 430,
            y: 440,
            region: "Dhaka",
            image: "https://images.unsplash.com/photo-1588880331179-bc9b93a8cb5e?w=1600&q=85",
            tagline: "Historic Mughal Capital of 400+ Years",
            attractions: ["Lalbagh Fort", "Ahsan Manzil (Pink Palace)", "National Parliament House", "Hatirjheel Promenade", "Tara Mosque"]
        },
        {
            name: "Gazipur",
            x: 440,
            y: 390,
            region: "Dhaka",
            image: "https://images.unsplash.com/photo-1448375240586-882707db888b?w=1600&q=85",
            tagline: "Eco-Resorts & Bhawal National Forest",
            attractions: ["Bhawal National Sal Forest", "Bangabandhu Safari Park", "Nuhash Polli", "Turag Riverfront Resorts"]
        },
        {
            name: "Narayanganj",
            x: 450,
            y: 470,
            region: "Dhaka",
            image: "https://images.unsplash.com/photo-1508873696983-2df5293cb32f?w=1600&q=85",
            tagline: "Historic Panam City & Ancient Bengal Capital",
            attractions: ["Sonargaon Folk Art Museum", "Panam Historic Ghost Town", "Hajiganj Fort", "Zinda Park Eco-Village"]
        },
        {
            name: "Narsingdi",
            x: 490,
            y: 410,
            region: "Dhaka",
            image: "https://images.unsplash.com/photo-1500382017468-9049fed747ef?w=1600&q=85",
            tagline: "2,500-Year Ancient Civilization of Wari-Bateshwar",
            attractions: ["Wari-Bateshwar Fort Ruins", "Dream Holiday Park", "Sonaimuri Hills", "Meghna River Ghat"]
        },
        {
            name: "Pabna",
            x: 270,
            y: 390,
            region: "Rajshahi",
            image: "https://images.unsplash.com/photo-1470071459604-3b5ec3a7fe05?w=1600&q=85",
            tagline: "Hardinge Historic Railway Bridge & Suchitra Sen Home",
            attractions: ["Hardinge British Railway Bridge", "Paksey Lalon Shah Bridge", "Tarash Zamindar Estate", "Mental Health Heritage"]
        },
        {
            name: "Kushtia",
            x: 230,
            y: 440,
            region: "Khulna",
            image: "https://images.unsplash.com/photo-1511497584788-87676104235f?w=1600&q=85",
            tagline: "Baul Shrine of Fakir Lalon Shah & Tagore Kuthibari",
            attractions: ["Lalon Shah Mystical Shrine", "Shilaidaha Rabindra Kuthibari", "Gorai Riverfront", "Jhaudia Mosque"]
        },
        {
            name: "Saidpur",
            x: 230,
            y: 140,
            region: "Rangpur",
            image: "https://images.unsplash.com/photo-1476514525535-07fb3b4ae5f1?w=1600&q=85",
            tagline: "Chini Masjid China Mosaic & Railway Heritage",
            attractions: ["Chini Glass Mosaic Mosque", "Old British Railway Workshops", "Teesta Barrage Excursion"]
        },
        {
            name: "Jamalpur",
            x: 410,
            y: 260,
            region: "Mymensingh",
            image: "https://images.unsplash.com/photo-1519681393784-d120267933ba?w=1600&q=85",
            tagline: "Jamuna River Charlands & Garo Hill Foothills",
            attractions: ["Lauchapra Eco Park", "Jamuna Riverfront Promenade", "Hazrat Shah Jamal Shrine", "Dhanua Border Point"]
        }
    ],

    connections: [
        { from: "Dhaka", to: "Chittagong", distance: 243 },
        { from: "Dhaka", to: "Khulna", distance: 299 },
        { from: "Dhaka", to: "Rajshahi", distance: 249 },
        { from: "Dhaka", to: "Comilla", distance: 114 },
        { from: "Dhaka", to: "Sylhet", distance: 240 },
        { from: "Dhaka", to: "Barisal", distance: 193 },
        { from: "Dhaka", to: "Rangpur", distance: 285 },
        { from: "Dhaka", to: "Narayanganj", distance: 17 },
        { from: "Dhaka", to: "Gazipur", distance: 38 },
        { from: "Dhaka", to: "Mymensingh", distance: 115 },
        { from: "Dhaka", to: "Jessore", distance: 169 },
        { from: "Dhaka", to: "Tangail", distance: 68 },
        { from: "Dhaka", to: "Bogra", distance: 193 },
        { from: "Dhaka", to: "Cox's Bazar", distance: 389 },
        { from: "Dhaka", to: "Narsingdi", distance: 53 },
        { from: "Dhaka", to: "Dinajpur", distance: 352 },
        { from: "Dhaka", to: "Pabna", distance: 192 },
        { from: "Dhaka", to: "Kushtia", distance: 246 },
        { from: "Dhaka", to: "Saidpur", distance: 295 },
        { from: "Dhaka", to: "Jamalpur", distance: 192 },

        { from: "Chittagong", to: "Cox's Bazar", distance: 152 },
        { from: "Chittagong", to: "Comilla", distance: 151 },
        { from: "Chittagong", to: "Sylhet", distance: 308 },
        { from: "Chittagong", to: "Barisal", distance: 311 },
        { from: "Chittagong", to: "Gazipur", distance: 258 },
        { from: "Chittagong", to: "Narayanganj", distance: 225 },

        { from: "Khulna", to: "Jessore", distance: 65 },
        { from: "Khulna", to: "Barisal", distance: 56 },
        { from: "Khulna", to: "Kushtia", distance: 92 },
        { from: "Khulna", to: "Rajshahi", distance: 229 },

        { from: "Rajshahi", to: "Pabna", distance: 73 },
        { from: "Rajshahi", to: "Bogra", distance: 79 },
        { from: "Rajshahi", to: "Kushtia", distance: 145 },
        { from: "Rajshahi", to: "Dinajpur", distance: 174 },
        { from: "Rajshahi", to: "Rangpur", distance: 195 },

        { from: "Rangpur", to: "Saidpur", distance: 121 },
        { from: "Rangpur", to: "Dinajpur", distance: 107 },
        { from: "Rangpur", to: "Bogra", distance: 88 },
        { from: "Rangpur", to: "Jamalpur", distance: 92 },

        { from: "Sylhet", to: "Mymensingh", distance: 365 },
        { from: "Sylhet", to: "Comilla", distance: 243 },

        { from: "Tangail", to: "Bogra", distance: 213 },
        { from: "Tangail", to: "Gazipur", distance: 152 },
        { from: "Tangail", to: "Mymensingh", distance: 225 },

        { from: "Bogra", to: "Dinajpur", distance: 49 },
        { from: "Bogra", to: "Saidpur", distance: 98 },
        { from: "Bogra", to: "Jamalpur", distance: 19 }
    ],

    packages: [
        {
            id: "PKG-101",
            name: "Cox's Bazar Beach & Marine Drive Holiday",
            destination: "Cox's Bazar",
            cost: 7500,
            durationDays: 3,
            durationNights: 2,
            hotelRating: "5-Star Sea Crown Resort",
            inclusions: "Complimentary Breakfast, Sunset Cruise, AC Scania Transport, Beach BBQ",
            description: "Experience the world's longest natural sea beach with luxury ocean view accommodation and scenic Marine Drive excursion.",
            image: "https://images.unsplash.com/photo-1596895111956-bf1cf0599ce5?w=800&q=80"
        },
        {
            id: "PKG-102",
            name: "Sylhet Ratargul & Tea Valley Expedition",
            destination: "Sylhet",
            cost: 5800,
            durationDays: 3,
            durationNights: 2,
            hotelRating: "4-Star Grand Sultan Tea Resort",
            inclusions: "All Meals, Swamp Forest Boat Ride, Jaflong Tour Guide, AC Hiace",
            description: "Explore lush green rolling tea gardens, the Ratargul freshwater swamp forest, and pristine crystal waters of Jaflong.",
            image: "https://images.unsplash.com/photo-1608958435020-e8a7109ba809?w=800&q=80"
        },
        {
            id: "PKG-103",
            name: "Sundarbans Wild Mangrove Safari",
            destination: "Khulna",
            cost: 12000,
            durationDays: 4,
            durationNights: 3,
            hotelRating: "Premium Cruise Ship Cabin",
            inclusions: "Buffet Dining, Armed Forest Guard, Watch Tower Trekking, Boat Safari",
            description: "Venture deep into UNESCO World Heritage Mangrove Forest, spot Royal Bengal Tigers, saltwater crocodiles & spotted deer.",
            image: "https://images.unsplash.com/photo-1544735716-392fe2489ffa?w=800&q=80"
        },
        {
            id: "PKG-104",
            name: "Heritage of Barisal & Floating Guava Market",
            destination: "Barisal",
            cost: 4500,
            durationDays: 2,
            durationNights: 1,
            hotelRating: "3-Star River View Heritage Hotel",
            inclusions: "Traditional Meals, Country Boat Tour, Sandhya River Cruise",
            description: "Witness traditional floating markets, rivers, backwaters, and peaceful rural beauty of southern Bangladesh.",
            image: "https://images.unsplash.com/photo-1507525428034-b723cf961d3e?w=800&q=80"
        },
        {
            id: "PKG-105",
            name: "North Bengal Archaeological Silk Trail",
            destination: "Rajshahi",
            cost: 5200,
            durationDays: 3,
            durationNights: 2,
            hotelRating: "4-Star Royal Rajshahi Palace",
            inclusions: "Breakfast & Dinner, Somapura Mahavihara & Puthia Palace Entry, AC Transport",
            description: "Explore ancient terracotta temples, historic silk factories, and unforgettable sunset over Padma River.",
            image: "https://images.unsplash.com/photo-1548013146-72479768bada?w=800&q=80"
        }
    ],

    users: [
        {
            username: "admin",
            passwordHash: "admin123",
            fullName: "System Administrator",
            phone: "+880 1700-112233",
            email: "admin@travelbd.gov.bd",
            role: "ADMIN",
            department: "Central Operations",
            accessLevel: 3
        },
        {
            username: "rahman",
            passwordHash: "pass123",
            fullName: "Anisur Rahman",
            phone: "+880 1712-345678",
            email: "rahman@gmail.com",
            address: "Dhanmondi 27, Dhaka",
            role: "CUSTOMER",
            walletBalance: 25000,
            loyaltyPoints: 250,
            tier: "GOLD",
            bookingHistory: []
        }
    ],

    coupons: {
        "OOP100": 1000,
        "HOLIDAY20": 0.20,
        "BANGLADESH": 0.15,
        "WELCOME50": 500
    },

    transportModes: [
        {
            type: "NON_AC_BUS",
            name: "Economy Non-AC Bus",
            speed: 45,
            rate: 1.80,
            icon: "fa-bus-simple",
            badge: "Budget Choice"
        },
        {
            type: "AC_BUS",
            name: "Executive AC Scania Coach",
            speed: 55,
            rate: 2.80,
            icon: "fa-bus",
            badge: "Most Popular"
        },
        {
            type: "EXPRESS_TRAIN",
            name: "Intercity Express Train",
            speed: 60,
            rate: 1.50,
            icon: "fa-train",
            badge: "Scenic & Reliable"
        },
        {
            type: "DOMESTIC_FLIGHT",
            name: "Domestic Air Flight",
            speed: 450,
            rate: 12.00,
            icon: "fa-plane-departure",
            badge: "Fastest Express",
            baseSurcharge: 2500
        }
    ]
};
