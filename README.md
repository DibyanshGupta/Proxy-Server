# Proxy Server

A multithreaded **C++17 Reverse Proxy** with JWT authentication, PostgreSQL-based user authentication, Argon2 password hashing, request routing, multiple load-balancing algorithms, backend health checking, thread pooling, and rate limiting.

## Architecture

```text
                         CLIENT
                           |
                           | HTTP
                           v
                 +----------------------+
                 |    REVERSE PROXY     |
                 |        :8080         |
                 +----------+-----------+
                            |
              +-------------+-------------+
              |                           |
              v                           v
       JWT Verification             Rate Limiter
              |                           |
              +-------------+-------------+
                            |
                            v
                      +-----------+
                      |  Router   |
                      +-----+-----+
                            |
                            v
                    +---------------+
                    | Load Balancer |
                    +-------+-------+
                            |
                 +----------+----------+
                 |                     |
                 v                     v
          User Service          Product Service
             :9001                  :9003


                    AUTHENTICATION
                           |
                           v
                    +-------------+
                    | Auth Service|
                    |    :9002    |
                    +------+------+
                           |
                           v
                    +-------------+
                    | PostgreSQL  |
                    | proxy_auth  |
                    +-------------+
```

## Services

| Service | Port | Purpose |
|---|---:|---|
| Reverse Proxy | `8080` | Main client entry point |
| User Service | `9001` | User backend |
| Auth Service | `9002` | Registration and login |
| Product Service | `9003` | Product backend |
| PostgreSQL | `5432` | User database |

## Project Structure

```text
Proxy/
│
├── ReverseProxy/
│   ├── include/
│   │   ├── BackendServer.h
│   │   ├── ClientHandler.h
│   │   ├── HealthChecker.h
│   │   ├── JwtVerifier.h
│   │   ├── LeastConnections.h
│   │   ├── LeastResponseTime.h
│   │   ├── LoadBalancer.h
│   │   ├── PerUserRateLimiter.h
│   │   ├── RateLimiter.h
│   │   ├── Router.h
│   │   ├── RoundRobin.h
│   │   ├── Server.h
│   │   ├── ServerPool.h
│   │   ├── SlidingWindow.h
│   │   ├── TaskQueue.h
│   │   ├── ThreadPool.h
│   │   ├── TokenBucket.h
│   │   └── WeightedRoundRobin.h
│   │
│   ├── src/
│   │   ├── ClientHandler.cpp
│   │   ├── HealthChecker.cpp
│   │   ├── JwtVerifier.cpp
│   │   ├── LeastConnections.cpp
│   │   ├── LeastResponseTime.cpp
│   │   ├── PerUserRateLimiter.cpp
│   │   ├── Router.cpp
│   │   ├── RoundRobin.cpp
│   │   ├── Server.cpp
│   │   ├── ServerPool.cpp
│   │   ├── SlidingWindow.cpp
│   │   ├── TaskQueue.cpp
│   │   ├── ThreadPool.cpp
│   │   ├── TokenBucket.cpp
│   │   ├── WeightedRoundRobin.cpp
│   │   └── main.cpp
│   │
│   └── Makefile
│
├── AuthService/
│   ├── include/
│   ├── src/
│   └── Makefile
│
├── user/
│   ├── include/
│   ├── src/
│   └── Makefile
│
├── product/
│   ├── include/
│   ├── src/
│   └── Makefile
│
└── README.md
```

## Request Flow

```text
Client
  |
  | GET /user
  | Authorization: Bearer <JWT>
  v
Reverse Proxy :8080
  |
  v
JWT Verification
  |
  +---- Invalid/Expired ----> 401 Unauthorized
  |
  v
Extract username from JWT
  |
  v
Per-User Rate Limiter
  |
  +---- Limit exceeded -----> 429 Too Many Requests
  |
  v
Router
  |
  v
Load Balancer
  |
  v
Healthy Backend
  |
  v
Backend Response
  |
  v
Client
```

## Routing

### User

Requests beginning with `/user` are routed to the User Service pool.

Current backend:

```text
127.0.0.1:9001
```

Example:

```bash
curl http://localhost:8080/user \
-H "Authorization: Bearer YOUR_TOKEN"
```

### Product

Requests beginning with `/product` are routed to the Product Service pool.

Current backend:

```text
127.0.0.1:9003
```

Example:

```bash
curl http://localhost:8080/product \
-H "Authorization: Bearer YOUR_TOKEN"
```

## Load Balancing

The project implements four load-balancing strategies.

### Round Robin

Distributes requests sequentially between healthy servers.

```text
Request 1 → Server A
Request 2 → Server B
Request 3 → Server C
Request 4 → Server A
```

### Least Connections

Selects the healthy backend with the fewest active connections.

```text
Server A → 5 connections
Server B → 2 connections
Server C → 8 connections

Next request → Server B
```

### Least Response Time

Selects the healthy backend with the lowest average response time.

The average response time is updated using:

```text
new_average =
    0.8 * old_average +
    0.2 * new_response_time
```

### Weighted Round Robin

Allows different backend servers to receive different proportions of traffic.

```text
Server A → weight 5
Server B → weight 2
Server C → weight 1
```

## Health Checking

The Reverse Proxy continuously checks backend availability.

Example:

```text
[HealthChecker] 127.0.0.1:9001 HEALTHY
[HealthChecker] 127.0.0.1:9003 HEALTHY
```

If a backend becomes unavailable, it is marked unhealthy and load balancers skip it.

When it becomes available again, it is marked healthy.

## Multithreading

The Reverse Proxy uses:

- `std::thread`
- Thread Pool
- Task Queue
- Mutexes
- Atomic variables

Architecture:

```text
Incoming Connections
        |
        v
   Server::accept()
        |
        v
    Task Queue
        |
   +----+----+----+
   |    |    |    |
   v    v    v    v
  T1   T2   T3   T4
   |    |    |    |
   +----+----+----+
        |
        v
   ClientHandler
```

This allows multiple client requests to be processed concurrently.

## Authentication

The Authentication Service runs on:

```text
localhost:9002
```

It provides:

```text
POST /register
POST /login
```

Passwords are hashed using **Argon2** and stored in PostgreSQL.

### Registration

```bash
curl -X POST http://localhost:9002/register \
-H "Content-Type: application/json" \
-d '{"username":"alice","password":"mypassword"}'
```

Response:

```json
{
    "message": "Registration successful"
}
```

### Login

```bash
curl -X POST http://localhost:9002/login \
-H "Content-Type: application/json" \
-d '{"username":"alice","password":"mypassword"}'
```

Response:

```json
{
    "message": "Login successful",
    "token": "YOUR_JWT_TOKEN"
}
```

## JWT Authentication

The JWT contains claims such as:

```json
{
    "sub": "alice",
    "role": "user",
    "iat": 1786308370,
    "exp": 1786311970
}
```

The Reverse Proxy verifies:

- JWT format
- Signature
- Expiration

A protected request must contain:

```text
Authorization: Bearer <JWT>
```

Example:

```bash
curl http://localhost:8080/user \
-H "Authorization: Bearer YOUR_TOKEN"
```

Invalid or expired tokens return:

```text
401 Unauthorized
```

## PostgreSQL

The authentication database is:

```text
proxy_auth
```

The main table is:

```sql
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(100) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role VARCHAR(50) NOT NULL DEFAULT 'user',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

Passwords are never stored as plaintext.

## Rate Limiting

The project provides a common:

```text
RateLimiter
```

interface with two implementations:

```text
RateLimiter
    |
    +-- SlidingWindow
    |
    +-- TokenBucket
```

The Reverse Proxy currently uses a **per-user Sliding Window rate limiter**.

### Per-User Rate Limiting

Rate limiting is performed **after successful JWT verification**.

The authenticated username is extracted from the JWT `sub` claim and is used as the key for an independent rate limiter.

```text
PerUserRateLimiter
    |
    +-- alice   → SlidingWindow
    |
    +-- bob     → SlidingWindow
    |
    +-- charlie → SlidingWindow
```

The current configuration is:

```text
10 requests per user
60-second window
```

Each user therefore has an independent request history.

For example:

```text
Alice reaches her limit → 429 Too Many Requests
Bob is still within his limit → 200 OK
```

Alice's requests do not consume Bob's rate-limit quota.

The rate-limit state is protected for concurrent access because multiple Reverse Proxy worker threads can process requests simultaneously.

### Sliding Window

The Sliding Window implementation tracks request timestamps and removes entries that have fallen outside the configured window.

Example configuration:

```text
10 requests
60 seconds
```

Once a user's limit is exceeded:

```text
429 Too Many Requests
```

Test with a valid JWT:

```bash
for i in {1..12}; do
    echo "Request $i"

    curl -s -o /dev/null -w "%{http_code}\n" \
    http://localhost:8080/user \
    -H "Authorization: Bearer YOUR_TOKEN"
done
```

Example result after a clean window:

```text
200
200
200
200
200
200
200
200
200
200
429
429
```

The exact point at which `429` begins depends on requests already made during the active 60-second window.

### Token Bucket

The Token Bucket implementation maintains:

- Capacity
- Current token count
- Refill rate
- Last update time

Each accepted request consumes one token.

The implementation is protected with a mutex for concurrent access.

## Thread Safety

Shared state is synchronized using:

```text
std::mutex
std::lock_guard
std::atomic
```

Thread-safe components include:

- Round Robin selection state
- Least Connections counters
- Least Response Time metrics
- Weighted Round Robin state
- Health checker running state
- Task queue
- Rate limiter state

## Requirements

The project is designed to run under Linux/WSL.

Required:

```text
C++17
g++
make
PostgreSQL
libpqxx
libpq
Argon2
OpenSSL
pthread
curl
```

Install dependencies:

```bash
sudo apt update

sudo apt install build-essential make

sudo apt install postgresql postgresql-contrib

sudo apt install libpqxx-dev

sudo apt install libargon2-dev

sudo apt install libssl-dev
```

## PostgreSQL Setup

Start PostgreSQL:

```bash
sudo service postgresql start
```

Create the database:

```bash
sudo -u postgres psql
```

```sql
CREATE DATABASE proxy_auth;
```

Connect:

```sql
\c proxy_auth
```

Create the table:

```sql
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(100) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role VARCHAR(50) NOT NULL DEFAULT 'user',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

Create the application user:

```sql
CREATE USER proxy_app WITH PASSWORD 'proxy_app_password';
```

Grant the required permissions to `proxy_app`.

Test:

```bash
psql -U proxy_app -d proxy_auth -h localhost
```

## Build

### Authentication Service

```bash
cd AuthService
make clean
make
```

Run:

```bash
./auth_service
```

### User Service

```bash
cd user
make
./user_service
```

### Product Service

```bash
cd product
make
./product_server
```

### Reverse Proxy

```bash
cd ReverseProxy
make clean
make
```

Run:

```bash
./reverse_proxy
```

## Running the Complete System

Start the services in this order:

### 1. PostgreSQL

```bash
sudo service postgresql start
```

### 2. Authentication Service

```bash
cd AuthService
./auth_service
```

Port:

```text
9002
```

### 3. User Service

```bash
cd user
./user_service
```

Port:

```text
9001
```

### 4. Product Service

```bash
cd product
./product_server
```

Port:

```text
9003
```

### 5. Reverse Proxy

```bash
cd ReverseProxy
./reverse_proxy
```

Port:

```text
8080
```

## End-to-End Test

Register:

```bash
curl -X POST http://localhost:9002/register \
-H "Content-Type: application/json" \
-d '{"username":"alice","password":"mypassword"}'
```

Login:

```bash
curl -X POST http://localhost:9002/login \
-H "Content-Type: application/json" \
-d '{"username":"alice","password":"mypassword"}'
```

Copy the returned JWT.

Then:

```bash
curl http://localhost:8080/user \
-H "Authorization: Bearer YOUR_TOKEN"
```

Expected response:

```json
{
    "service": "User Service",
    "status": "success",
    "user": "John Doe"
}
```

## Error Responses

### Invalid Authentication

```text
401 Unauthorized
```

### Rate Limit Exceeded

```text
429 Too Many Requests
```

### No Healthy Backend

```text
503 Service Unavailable
```

## Project Status

| Component | Status |
|---|---|
| Reverse Proxy | Complete |
| HTTP Request Handling | Complete |
| Thread Pool | Complete |
| Task Queue | Complete |
| User Service | Complete |
| Product Service | Complete |
| Request Routing | Complete |
| Round Robin | Complete |
| Least Connections | Complete |
| Least Response Time | Complete |
| Weighted Round Robin | Complete |
| Backend Health Checking | Complete |
| PostgreSQL Authentication | Complete |
| User Registration | Complete |
| Argon2 Password Hashing | Complete |
| JWT Generation | Complete |
| JWT Verification | Complete |
| Sliding Window Rate Limiting | Complete |
| Token Bucket | Complete |
| Per-User Rate Limiting | Complete |

## Future Improvements

Possible future improvements include:

- HTTPS/TLS
- HTTP health-check endpoints
- Graceful shutdown
- Configuration files
- Environment-based secrets
- Connection pooling
- Distributed rate limiting
- Metrics and monitoring
- Automated unit/integration tests
- Containerization
- Production deployment configuration

## Technologies

```text
C++17
POSIX Sockets
std::thread
Thread Pool
PostgreSQL
libpqxx
Argon2
OpenSSL
JWT
HTTP
Git
Linux / WSL
```

## Repository

GitHub:

https://github.com/DibyanshGupta/Proxy-Server

## Authors

**Dibyansh Gupta**

**Sorithiya Aditya Vijaybhai**

---

This project demonstrates the implementation and integration of a multithreaded reverse proxy with authentication, per-user rate limiting, load balancing, backend health monitoring, and rate limiting algorithms in C++.
