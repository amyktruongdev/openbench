import React from 'react';
import Nav from '../Nav';
import Footer from '../Footer';
import Time from '../Time';


const Add = () => {
  return (
    <div>
      <Nav /> {/* Navbar will stay fixed on the left */}
      <div className="content">

        <div className="box-container">
        <h1 className="header">OpenBench</h1>
        <Time />
          <div className="row">
            <div className="box">
              <h2>Gym Overall</h2>
              <h2 className="amount">15</h2>
              <div className="word-spacing">Last updated 4:00pm</div>
            </div>
            <div className="box">
              <h2>Treadmill</h2>
              <h2 className="amount">15</h2>
              <div className="word-spacing">Last updated 4:00pm</div>
            </div>
          </div>
          <div className="row">
            <div className="box">
              <h2>Bench</h2>
              <h2 className="amount">15</h2>
              <div className="word-spacing">Last updated 4:00pm</div>
            </div>
            <div className="box">
              <h2>Weights</h2>
              <h2 className="amount">15</h2>
              <div className="word-spacing">Last updated 4:00pm</div>
            </div>
          </div>
        </div>
        <Footer />
      </div>
    </div>
  );
};

export default Add;
