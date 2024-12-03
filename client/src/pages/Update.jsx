import React from 'react';
import { Link } from 'react-router-dom';

const Update = () => {
  return (
    <div>
      <h1>OpenBench Login</h1>
      <div className="container">
        <label htmlFor="uname"><b>Username</b></label>
        <input type="text" placeholder="Enter Username" name="uname" required />

        <label htmlFor="psw"><b>Password</b></label>
        <input type="password" placeholder="Enter Password" name="psw" required />

        <button type="submit"><Link to="/add">Login</Link></button>
        <label>
          <input type="checkbox" defaultChecked name="remember" /> Remember me
        </label>
      </div>

      <div className="container" style={{ backgroundColor: "#f1f1f1" }}>
        <button type="button" className="cancelbtn">Cancel</button>
        <span className="psw">Forgot <a href="#">password?</a></span>
      </div>
    </div>
  );
};

export default Update;
