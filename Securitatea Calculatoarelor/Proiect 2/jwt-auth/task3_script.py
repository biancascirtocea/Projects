from datetime import datetime, timedelta, timezone
import jwt

# Copiem SECRET_KEY din app.py
SECRET_KEY = "aAveTNQmtDOrrm0PdJnV5jAyjUl4eIHe"

# Modificăm funcția create_token pentru a seta isAdmin = True
def create_admin_token(username):
    payload = {
        "username": "testuser123",  # username-ul folosit la register
        "isAdmin": True,  # modificat la True pentru a obține acces admin
        "exp": datetime.now(tz=timezone.utc) + timedelta(minutes=15),
    }
    token = jwt.encode(payload, SECRET_KEY, algorithm="HS256")
    return token

# Generăm și afișăm token-ul
token = create_admin_token("testuser123")
print("Token:", token)
