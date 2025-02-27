import React from 'react';
import { Link } from 'react-router-dom';
import '../Login.css'; // Import the updated CSS

const Update = () => {
  return (
    <div className="login-wrapper">
      <div className="login-card">
        <h1 className="login-header">OpenBench Login</h1>
        <form className="login-form">
          <div className="input-group">
            <label htmlFor="uname">Username</label>
            <input type="text" id="uname" placeholder="Enter Username" required />
          </div>
          <div className="input-group">
            <label htmlFor="psw">Password</label>
            <input type="password" id="psw" placeholder="Enter Password" required />
          </div>
          <div className="options">
            <label className="remember">
              <input type="checkbox" defaultChecked name="remember" /> Remember me
            </label>
            <Link to="/forgot-password" className="forgot-link">
              Forgot password?
            </Link>
          </div>
          <div className="actions">
            <button type="submit" className="btn login-btn">
              Login
            </button>
            <Link to="/registration" className="btn register-btn">
              Create Account
            </Link>
          </div>
        </form>
      </div>
    </div>
  );
};

export default Update;
