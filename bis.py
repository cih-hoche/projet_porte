from fastapi import FastAPI
from fastapi import HTTPException
from pydantic import BaseModel
from sqlalchemy import create_engine, Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

# Define the database connection URL
DATABASE_URL = "sqlite:///portebis.db"

# Create the SQLAlchemy engine
engine = create_engine(DATABASE_URL)

# Create a session factory
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

# Create the base model for database tables
Base = declarative_base()

# Define your database model
class User(Base):
    __tablename__ = "utilisateurs"

    id = Column(Integer, primary_key=True, index=True, )
    name = Column(String, index=True)
    status = Column(String)

# Create the FastAPI app
app = FastAPI()

# Define the request body model
class DoorCreate(BaseModel):
    name: str
    status: str

# Create a new door
@app.post("/doors/")
def create_door(door: DoorCreate):
    db = SessionLocal()
    new_door = Door(name=door.name, status=door.status)
    db.add(new_door)
    db.commit()
    db.refresh(new_door)
    return new_door

# Get all doors
@app.get("/doors/")
def get_doors():
    db = SessionLocal()
    doors = db.query(Door).all()
    return doors

# Get a specific door by ID
@app.get("/doors/{door_id}")
def get_door(door_id: int):
    db = SessionLocal()
    door = db.query(Door).filter(Door.id == door_id).first()
    if not door:
        raise HTTPException(status_code=404, detail="Door not found")
    return door