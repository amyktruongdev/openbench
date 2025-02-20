import React from 'react';
import Time from '../Time';
import Nav from '../Nav';
import '../Map.css';

const Map = () => {
  return (
    <div>
      <h1>OpenBench</h1>
      <Time />
      <Nav></Nav>
      <div className="map-container">
        <img src="/map.png" alt="Map Image" className="map-image" />
        <div className="overlay-shape"></div>
        {/* New Shape to the right */}
        <div className="overlay-shape right-shape" />
        <div className="overlay-shape right-shape-two" />
        <div className="overlay-shape right-shape-three" />
        <div className="overlay-shape right-shape-four" />
        <div className="overlay-shape right-shape-five" />

        {/* Second row of shapes directly below */}
        <div className="overlay-shape second-row-shape" />
        <div className="overlay-shape second-row-shape-two" />
        <div className="overlay-shape second-row-shape-three" />
        <div className="overlay-shape second-row-shape-four" />
        <div className="overlay-shape second-row-shape-five" />
      </div>
    </div>
  );
};

export default Map;
