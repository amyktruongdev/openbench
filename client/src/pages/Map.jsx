import React from 'react';
import Time from '../Time';
import Nav from '../Nav';
import Footer from '../Footer';
import '../Map.css';

const Map = () => {
  return (
    <div>
      <h1 className="header">OpenBench</h1>
      <Time />
      <Nav />
      {/*Useable shapes front-->back*/}
      <div className="map-container">
        <img src="/map.png" alt="Map Image" className="map-image" />
        <div className="overlay-shape"></div>
        {/* First row of shapes */}
        <div className="overlay-shape right-shape" />
        <div className="overlay-shape right-shape-two" />
        <div className="overlay-shape right-shape-three" />
        <div className="overlay-shape right-shape-four" />
        <div className="overlay-shape right-shape-five" />

        {/* Second row of shapes */}
        <div className="overlay-shape second-row-shape" />
        <div className="overlay-shape second-row-shape-two" />
        <div className="overlay-shape second-row-shape-three" />
        <div className="overlay-shape second-row-shape-four" />
        <div className="overlay-shape second-row-shape-five" />

        {/* Map Legend */}
        <div className="map-legend">
          <h3>Equipment Status</h3>
          <ul>
            <li>
              <span className="legend-color available"></span> Available
            </li>
            <li>
              <span className="legend-color taken"></span> Taken
            </li>
            <li>
              <span className="legend-color maintenance"></span> Under Maintenance
            </li>
          </ul>
        </div>
      </div>
      <Footer></Footer>
    </div>
  );
};

export default Map;
