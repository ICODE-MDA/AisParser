CREATE TABLE IF NOT EXISTS vessels (
  key_column BIGSERIAL PRIMARY KEY,
  MessageType float8 DEFAULT NULL, 
  MMSI float8 NOT NULL DEFAULT '0',
  NavStatus numeric(10,0) DEFAULT 0,
  ROT float8 DEFAULT NULL,
  SOG float8 DEFAULT NULL,
  LON float8 DEFAULT NULL,
  LAT float8 DEFAULT NULL,
  COG float8 DEFAULT NULL,
  true_heading float8 DEFAULT NULL,
  DateTime float8 DEFAULT NULL, 
  IMO float8 DEFAULT NULL,
  VesselName varchar(128) DEFAULT NULL,
  VesselTypeInt float8 DEFAULT NULL,
  Length float8 DEFAULT NULL,
  shipWidth float8 DEFAULT NULL, 
  Bow float8 DEFAULT NULL, 
  Stern float8 DEFAULT NULL, 
  Port float8 DEFAULT NULL, 
  Starboard float8 DEFAULT NULL, 
  Draught float8 DEFAULT NULL, 
  Destination VARCHAR(128) DEFAULT NULL, 
  Callsign VARCHAR(128) DEFAULT NULL, 
  PosAccuracy float8 DEFAULT NULL, 
  ETA float8 DEFAULT NULL, 
  PosFixType float8 DEFAULT NULL,
  STREAMID VARCHAR(36) DEFAULT NULL
);

INSERT INTO vessels VALUES (DEFAULT , 1, 123456789, 'Trawling', 0.1, 0.2, 0.3, 0.4, 0.5, 90.0, 100526, 123456789, 'vessel name', 12, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 'Destination', 'callsign', 3, 102612, 39, 'orbcomm' );

