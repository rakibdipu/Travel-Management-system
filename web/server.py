#!/usr/bin/env python3
"""
===============================================================================
Bangladesh Tourism & Travel Management System - SQLite3 Backend API & Server
===============================================================================
Provides a lightweight RESTful API with SQLite database persistence,
automatic file synchronization with C++ data files (data/*.txt),
and static file serving.
"""

import http.server
import socketserver
import json
import sqlite3
import os
import sys
import urllib.parse
import webbrowser

PORT = 8080
WEB_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(WEB_DIR, ".."))
DB_DIR = os.path.join(BASE_DIR, "database")
DB_PATH = os.path.join(DB_DIR, "travelbd.db")
DATA_DIR = os.path.join(BASE_DIR, "data")

os.makedirs(DB_DIR, exist_ok=True)
os.makedirs(DATA_DIR, exist_ok=True)

# =============================================================================
# 1. SQLITE DATABASE INITIALIZATION & SCHEMA
# =============================================================================
def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    conn = get_db()
    cursor = conn.cursor()

    # Users Table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS users (
        username TEXT PRIMARY KEY,
        password_hash TEXT NOT NULL,
        full_name TEXT NOT NULL,
        phone TEXT,
        email TEXT,
        address TEXT,
        role TEXT DEFAULT 'CUSTOMER',
        wallet_balance REAL DEFAULT 0.0,
        loyalty_points INTEGER DEFAULT 0,
        tier TEXT DEFAULT 'SILVER',
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    """)

    # Bookings Table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS bookings (
        booking_id TEXT PRIMARY KEY,
        username TEXT NOT NULL,
        customer_name TEXT NOT NULL,
        phone TEXT,
        trip_type TEXT NOT NULL,
        trip_title TEXT NOT NULL,
        origin TEXT NOT NULL,
        destination TEXT NOT NULL,
        route_path TEXT,
        transport_name TEXT NOT NULL,
        distance_km INTEGER DEFAULT 0,
        booking_date TEXT NOT NULL,
        base_cost REAL NOT NULL,
        discount_amount REAL DEFAULT 0.0,
        coupon_code TEXT,
        vat_amount REAL DEFAULT 0.0,
        final_cost REAL NOT NULL,
        status TEXT DEFAULT 'CONFIRMED',
        payment_method TEXT NOT NULL,
        transaction_id TEXT NOT NULL,
        FOREIGN KEY(username) REFERENCES users(username)
    )
    """)

    # Packages Table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS packages (
        id TEXT PRIMARY KEY,
        name TEXT NOT NULL,
        destination TEXT NOT NULL,
        duration_days INTEGER NOT NULL,
        duration_nights INTEGER NOT NULL,
        cost REAL NOT NULL,
        hotel_rating TEXT,
        description TEXT,
        inclusions TEXT,
        image TEXT
    )
    """)

    # Coupons Table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS coupons (
        code TEXT PRIMARY KEY,
        discount_value REAL NOT NULL,
        is_percentage INTEGER DEFAULT 0
    )
    """)

    # Highway Connections Table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS connections (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        from_city TEXT NOT NULL,
        to_city TEXT NOT NULL,
        distance_km INTEGER NOT NULL
    )
    """)

    # Pre-seed Admin and Default Customer if not exists
    cursor.execute("SELECT COUNT(*) FROM users WHERE username = 'admin'")
    if cursor.fetchone()[0] == 0:
        cursor.execute("""
        INSERT INTO users (username, password_hash, full_name, phone, email, address, role, wallet_balance, loyalty_points, tier)
        VALUES ('admin', 'admin123', 'System Administrator', '+880 1700-112233', 'admin@travelbd.gov.bd', 'Central Operations, Dhaka', 'ADMIN', 0.0, 0, 'ADMIN')
        """)

    cursor.execute("SELECT COUNT(*) FROM users WHERE username = 'rahman'")
    if cursor.fetchone()[0] == 0:
        cursor.execute("""
        INSERT INTO users (username, password_hash, full_name, phone, email, address, role, wallet_balance, loyalty_points, tier)
        VALUES ('rahman', 'pass123', 'Anisur Rahman', '+880 1712-345678', 'rahman@gmail.com', 'Dhanmondi 27, Dhaka', 'CUSTOMER', 25000.0, 350, 'GOLD')
        """)

    # Pre-seed Coupons
    coupons_data = [
        ('OOP100', 1000.0, 0),
        ('HOLIDAY20', 0.20, 1),
        ('BANGLADESH', 0.15, 1),
        ('WELCOME50', 500.0, 0)
    ]
    for c, val, is_p in coupons_data:
        cursor.execute("INSERT OR IGNORE INTO coupons (code, discount_value, is_percentage) VALUES (?, ?, ?)", (c, val, is_p))

    # Pre-seed Packages
    packages_data = [
        ('PKG-01', "Cox's Bazar Beach & Marine Drive Holiday", "Cox's Bazar", 4, 3, 7500, "★★★★★ 5-Star Sea Crown Resort", "Relax at the world's longest unbroken sea beach with sunset views and Marine Drive tour.", "AC Transport, Resort Stay, Buffet Breakfast, Marine Drive Safari", "https://images.unsplash.com/photo-1596895111956-bf1cf0599ce5?w=800&q=80"),
        ('PKG-02', "Sylhet Tea Valley & Ratargul Eco Adventure", "Sylhet", 3, 2, 5800, "★★★★☆ Grand Sylhet Eco Resort", "Explore sprawling green tea gardens, Jaflong stone river, and Ratargul freshwater swamp.", "AC Coach, Eco Cottage, Boat Safari, 3-Course Meals", "https://images.unsplash.com/photo-1608958435020-e8a7109ba809?w=800&q=80"),
        ('PKG-03', "Sundarbans Wild Mangrove & Tiger Cruise", "Khulna", 4, 3, 9500, "★★★★★ Luxury Cruise Vessel M.V. Bengal", "Deep jungle safari in the UNESCO World Heritage mangrove forest with forest ranger security.", "Cabin AC Cruise, Forest Entry Pass, Armed Guards, All Meals", "https://images.unsplash.com/photo-1544735716-392fe2489ffa?w=800&q=80"),
        ('PKG-04', "Sajek Valley Kingdom of Clouds Experience", "Chittagong", 3, 2, 6200, "★★★★☆ Sajek Hill View Resort", "Experience floating above the clouds, Konglak Para highest peak, and tribal cultural music.", "Chander Gari 4x4, Mountain Resort, Tribal BBQ Dinner, Guided Trek", "https://images.unsplash.com/photo-1506744038136-46273834b3fb?w=800&q=80"),
        ('PKG-05', "Paharpur & Mahasthangarh Heritage Voyage", "Rajshahi", 3, 2, 4900, "★★★★☆ Silk City Heritage Hotel", "Discover Somapura Mahavihara Buddhist ruins and 3rd century BCE archaeological relics.", "AC Tourist Bus, Heritage Hotel, Museum Tickets, Expert Historian Guide", "https://images.unsplash.com/photo-1590523741831-ab7e8b8f9c7f?w=800&q=80"),
        ('PKG-06', "Kuakata Daughter of the Sea Sunrise & Sunset", "Barisal", 3, 2, 5400, "★★★★☆ Kuakata Grand Beach Resort", "Watch both magnificent sunrise and sunset over the Bay of Bengal from the same sandy beach.", "AC Travel Coach, Beach Resort, Red Crab Island Boat Safari, Meals", "https://images.unsplash.com/photo-1507525428034-b723cf961d3e?w=800&q=80")
    ]
    for p in packages_data:
        cursor.execute("""
        INSERT OR IGNORE INTO packages (id, name, destination, duration_days, duration_nights, cost, hotel_rating, description, inclusions, image)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, p)

    conn.commit()
    conn.close()
    sync_db_to_files()

# =============================================================================
# 2. AUTO-SYNC WITH C++ DATA FILES
# =============================================================================
def sync_db_to_files():
    """Synchronizes SQLite database contents with data/*.txt files used by C++ engine."""
    try:
        conn = get_db()
        cursor = conn.cursor()

        # 1. Sync users.txt
        users_file = os.path.join(DATA_DIR, "users.txt")
        cursor.execute("SELECT * FROM users")
        user_rows = cursor.fetchall()
        with open(users_file, "w", encoding="utf-8") as f:
            f.write("# User Accounts Database\n")
            for u in user_rows:
                # Format: Role|Username|PasswordHash|FullName|Phone|Email|Address|WalletBalance|LoyaltyPoints|Bookings
                f.write(f"{u['role']}|{u['username']}|{u['password_hash']}|{u['full_name']}|{u['phone']}|{u['email']}|{u['address']}|{u['wallet_balance']}|{u['loyalty_points']}|\n")

        # 2. Sync bookings.txt
        bookings_file = os.path.join(DATA_DIR, "bookings.txt")
        cursor.execute("SELECT * FROM bookings")
        b_rows = cursor.fetchall()
        with open(bookings_file, "w", encoding="utf-8") as f:
            f.write("# Bookings Registry\n")
            for b in b_rows:
                f.write(f"{b['booking_id']}|{b['username']}|{b['customer_name']}|{b['phone']}|{b['trip_type']}|{b['trip_title']}|{b['origin']}|{b['destination']}|{b['route_path']}|{b['transport_name']}|{b['distance_km']}|{b['booking_date']}|{b['base_cost']}|{b['discount_amount']}|{b['coupon_code'] or 'NONE'}|{b['vat_amount']}|{b['final_cost']}|{1 if b['status'] == 'CANCELLED' else 0}|{b['payment_method']}|{b['transaction_id']}\n")

        conn.close()
    except Exception as e:
        print(f"[Sync Warning] {e}")

# =============================================================================
# 3. HTTP REST API HANDLER
# =============================================================================
class APIHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def _send_json(self, data, status=200):
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
        self.wfile.write(json.dumps(data).encode("utf-8"))

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        if path.startswith("/api/"):
            conn = get_db()
            cursor = conn.cursor()

            if path == "/api/status":
                self._send_json({
                    "status": "connected",
                    "database": "SQLite 3",
                    "db_file": DB_PATH,
                    "version": sqlite3.sqlite_version
                })

            elif path == "/api/data/bootstrap":
                # Returns complete pre-loaded database state for frontend initialization
                cursor.execute("SELECT username, password_hash as passwordHash, full_name as fullName, phone, email, address, role, wallet_balance as walletBalance, loyalty_points as loyaltyPoints, tier FROM users")
                users = [dict(row) for row in cursor.fetchall()]

                cursor.execute("SELECT booking_id as bookingId, username, customer_name as customerName, phone, trip_type as tripType, trip_title as tripTitle, origin, destination, route_path as routePath, transport_name as transportName, distance_km as distanceKm, booking_date as bookingDate, base_cost as baseCost, discount_amount as discountAmount, coupon_code as couponCode, vat_amount as vatAmount, final_cost as finalCost, status, payment_method as paymentMethod, transaction_id as transactionId FROM bookings ORDER BY booking_date DESC")
                bookings = [dict(row) for row in cursor.fetchall()]

                cursor.execute("SELECT id, name, destination, duration_days as durationDays, duration_nights as durationNights, cost, hotel_rating as hotelRating, description, inclusions, image FROM packages")
                packages = [dict(row) for row in cursor.fetchall()]

                cursor.execute("SELECT code, discount_value, is_percentage FROM coupons")
                coupons = {row["code"]: row["discount_value"] for row in cursor.fetchall()}

                conn.close()
                self._send_json({
                    "success": True,
                    "users": users,
                    "bookings": bookings,
                    "packages": packages,
                    "coupons": coupons
                })

            elif path == "/api/users":
                cursor.execute("SELECT username, full_name as fullName, phone, email, address, role, wallet_balance as walletBalance, loyalty_points as loyaltyPoints, tier FROM users")
                users = [dict(row) for row in cursor.fetchall()]
                conn.close()
                self._send_json({"success": True, "users": users})

            elif path == "/api/bookings":
                cursor.execute("SELECT * FROM bookings ORDER BY booking_date DESC")
                bookings = [dict(row) for row in cursor.fetchall()]
                conn.close()
                self._send_json({"success": True, "bookings": bookings})

            elif path == "/api/packages":
                cursor.execute("SELECT * FROM packages")
                packages = [dict(row) for row in cursor.fetchall()]
                conn.close()
                self._send_json({"success": True, "packages": packages})

            else:
                conn.close()
                self._send_json({"error": "Endpoint not found"}, status=404)
        else:
            super().do_GET()

    def do_POST(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        if path.startswith("/api/"):
            content_length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(content_length).decode("utf-8")
            data = json.loads(body) if body else {}

            conn = get_db()
            cursor = conn.cursor()

            try:
                # 1. User Registration -> Database
                if path == "/api/users/register":
                    uname = data.get("username", "").strip()
                    pass_hash = data.get("password", "").strip()
                    name = data.get("fullName", "").strip()
                    phone = data.get("phone", "").strip()
                    email = data.get("email", "").strip()
                    address = data.get("address", "").strip()

                    cursor.execute("SELECT COUNT(*) FROM users WHERE username = ?", (uname,))
                    if cursor.fetchone()[0] > 0:
                        conn.close()
                        self._send_json({"success": False, "error": "Username already registered!"}, status=400)
                        return

                    cursor.execute("""
                    INSERT INTO users (username, password_hash, full_name, phone, email, address, role, wallet_balance, loyalty_points, tier)
                    VALUES (?, ?, ?, ?, ?, ?, 'CUSTOMER', 500.0, 50, 'SILVER')
                    """, (uname, pass_hash, name, phone, email, address))
                    conn.commit()

                    cursor.execute("SELECT username, full_name as fullName, phone, email, address, role, wallet_balance as walletBalance, loyalty_points as loyaltyPoints, tier FROM users WHERE username = ?", (uname,))
                    user = dict(cursor.fetchone())
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": "User registered in SQLite DB successfully!", "user": user})

                # 2. User Login -> Database
                elif path == "/api/users/login":
                    uname = data.get("username", "").strip()
                    password = data.get("password", "").strip()

                    cursor.execute("SELECT username, password_hash, full_name as fullName, phone, email, address, role, wallet_balance as walletBalance, loyalty_points as loyaltyPoints, tier FROM users WHERE username = ?", (uname,))
                    row = cursor.fetchone()
                    if not row:
                        conn.close()
                        self._send_json({"success": False, "error": "User account not found!"}, status=404)
                        return

                    user = dict(row)
                    if user["password_hash"] != password:
                        conn.close()
                        self._send_json({"success": False, "error": "Incorrect password!"}, status=401)
                        return

                    del user["password_hash"]
                    conn.close()
                    self._send_json({"success": True, "user": user})

                # 3. Trip / Package Booking -> Database
                elif path == "/api/bookings/create":
                    b_id = data.get("bookingId")
                    uname = data.get("username")
                    cname = data.get("customerName")
                    phone = data.get("phone")
                    ttype = data.get("tripType")
                    title = data.get("tripTitle")
                    origin = data.get("origin")
                    dest = data.get("destination")
                    route = data.get("routePath")
                    tname = data.get("transportName")
                    dist = data.get("distanceKm", 0)
                    bdate = data.get("bookingDate")
                    base = data.get("baseCost", 0)
                    disc = data.get("discountAmount", 0)
                    coupon = data.get("couponCode", "")
                    vat = data.get("vatAmount", 0)
                    final_cost = data.get("finalCost", 0)
                    status = data.get("status", "CONFIRMED")
                    pay_method = data.get("paymentMethod")
                    tx_id = data.get("transactionId")

                    cursor.execute("""
                    INSERT INTO bookings (booking_id, username, customer_name, phone, trip_type, trip_title, origin, destination, route_path, transport_name, distance_km, booking_date, base_cost, discount_amount, coupon_code, vat_amount, final_cost, status, payment_method, transaction_id)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                    """, (b_id, uname, cname, phone, ttype, title, origin, dest, route, tname, dist, bdate, base, disc, coupon, vat, final_cost, status, pay_method, tx_id))

                    # Deduct from wallet if wallet payment
                    if pay_method == "WALLET":
                        cursor.execute("UPDATE users SET wallet_balance = wallet_balance - ? WHERE username = ?", (final_cost, uname))

                    # Reward loyalty points (1 point per 100 BDT)
                    points_earned = int(final_cost // 100)
                    cursor.execute("UPDATE users SET loyalty_points = loyalty_points + ? WHERE username = ?", (points_earned, uname))

                    # Auto upgrade tier
                    cursor.execute("SELECT wallet_balance, loyalty_points FROM users WHERE username = ?", (uname,))
                    u_row = cursor.fetchone()
                    if u_row:
                        new_tier = "PLATINUM" if u_row["loyalty_points"] >= 500 else ("GOLD" if u_row["loyalty_points"] >= 200 else "SILVER")
                        cursor.execute("UPDATE users SET tier = ? WHERE username = ?", (new_tier, uname))

                    conn.commit()
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": "Booking inserted into SQLite Database!", "bookingId": b_id})

                # 4. Cancel Booking & 90% Refund -> Database
                elif path == "/api/bookings/cancel":
                    b_id = data.get("bookingId")
                    cursor.execute("SELECT * FROM bookings WHERE booking_id = ?", (b_id,))
                    b_row = cursor.fetchone()
                    if not b_row:
                        conn.close()
                        self._send_json({"success": False, "error": "Booking not found!"}, status=404)
                        return

                    booking = dict(b_row)
                    refund = booking["final_cost"] * 0.90
                    cursor.execute("UPDATE bookings SET status = 'CANCELLED' WHERE booking_id = ?", (b_id,))
                    cursor.execute("UPDATE users SET wallet_balance = wallet_balance + ? WHERE username = ?", (refund, booking["username"]))
                    conn.commit()
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": f"Booking cancelled and {refund:.2f} BDT refunded to Travel Wallet!", "refund": refund})

                # 5. Wallet Top-Up -> Database
                elif path == "/api/users/update_wallet":
                    uname = data.get("username")
                    amount = float(data.get("amount", 0))
                    if amount <= 0:
                        conn.close()
                        self._send_json({"success": False, "error": "Invalid amount!"}, status=400)
                        return

                    cursor.execute("UPDATE users SET wallet_balance = wallet_balance + ? WHERE username = ?", (amount, uname))
                    conn.commit()
                    cursor.execute("SELECT wallet_balance FROM users WHERE username = ?", (uname,))
                    new_bal = cursor.fetchone()["wallet_balance"]
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": "Wallet topped up in SQLite DB!", "walletBalance": new_bal})

                # 6. Redeem Points -> Database
                elif path == "/api/users/redeem_points":
                    uname = data.get("username")
                    points = int(data.get("points", 0))
                    cash = points * 0.50

                    cursor.execute("SELECT loyalty_points FROM users WHERE username = ?", (uname,))
                    u_row = cursor.fetchone()
                    if not u_row or u_row["loyalty_points"] < points:
                        conn.close()
                        self._send_json({"success": False, "error": "Insufficient loyalty points!"}, status=400)
                        return

                    cursor.execute("UPDATE users SET loyalty_points = loyalty_points - ?, wallet_balance = wallet_balance + ? WHERE username = ?", (points, cash, uname))
                    conn.commit()
                    cursor.execute("SELECT wallet_balance, loyalty_points FROM users WHERE username = ?", (uname,))
                    updated = dict(cursor.fetchone())
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": f"{points} points redeemed for {cash:.2f} BDT!", "user": updated})

                # 7. Admin Add Tour Package -> Database
                elif path == "/api/packages/create":
                    pkg_id = data.get("id") or f"PKG-{os.urandom(3).hex().upper()}"
                    name = data.get("name")
                    dest = data.get("destination")
                    days = int(data.get("durationDays", 3))
                    nights = int(data.get("durationNights", 2))
                    cost = float(data.get("cost", 5000))
                    rating = data.get("hotelRating", "★★★★☆ Luxury Resort")
                    desc = data.get("description", "Exotic Bangladesh Tour")
                    inclusions = data.get("inclusions", "AC Coach, Hotel, Breakfast")
                    img = data.get("image", "https://images.unsplash.com/photo-1596895111956-bf1cf0599ce5?w=800&q=80")

                    cursor.execute("""
                    INSERT INTO packages (id, name, destination, duration_days, duration_nights, cost, hotel_rating, description, inclusions, image)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                    """, (pkg_id, name, dest, days, nights, cost, rating, desc, inclusions, img))
                    conn.commit()
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": "Package saved to SQLite Database!", "id": pkg_id})

                # 8. Admin Add Road Connection -> Database
                elif path == "/api/connections/create":
                    from_c = data.get("from")
                    to_c = data.get("to")
                    dist = int(data.get("distance", 0))

                    cursor.execute("INSERT INTO connections (from_city, to_city, distance_km) VALUES (?, ?, ?)", (from_c, to_c, dist))
                    conn.commit()
                    conn.close()
                    sync_db_to_files()

                    self._send_json({"success": True, "message": "Road connection saved to SQLite Database!"})

                else:
                    conn.close()
                    self._send_json({"error": "Unknown API endpoint"}, status=404)

            except Exception as ex:
                conn.close()
                self._send_json({"success": False, "error": str(ex)}, status=500)

def run_server():
    init_db()
    with socketserver.TCPServer(("", PORT), APIHandler) as httpd:
        url = f"http://localhost:{PORT}"
        print("=" * 65)
        print("  🇧🇩 Bangladesh Tourism & Travel Management System (Backend)")
        print(f"  [✓] SQLite3 Database: {DB_PATH}")
        print(f"  [✓] REST API & Web Server active at: {url}")
        print("  [✓] Live Sync with C++ Engine data files active.")
        print("  Press Ctrl+C to stop the server.")
        print("=" * 65)
        webbrowser.open(url)
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server...")

if __name__ == "__main__":
    run_server()
