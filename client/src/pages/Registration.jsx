import React from "react";
import { Link } from "react-router-dom";
import "./Registration.css"; // Import the updated CSS

const RegistrationPage = () => {
  return (
    <div className="registration-wrapper">
      <div className="registration-card">
        <h1 className="registration-header">Create Your Account</h1>
        <form className="registration-form">
          <div className="input-group">
            <label htmlFor="firstname">First Name</label>
            <input
              type="text"
              id="firstname"
              placeholder="Enter your first name"
              required
            />
          </div>
          <div className="input-group">
            <label htmlFor="lastname">Last Name</label>
            <input
              type="text"
              id="lastname"
              placeholder="Enter your last name"
              required
            />
          </div>
          <div className="input-group">
            <label htmlFor="uname">Username</label>
            <input
              type="text"
              id="uname"
              placeholder="Choose a username"
              required
            />
          </div>
          <div className="input-group">
            <label htmlFor="email">Email</label>
            <input
              type="email"
              id="email"
              placeholder="Enter your email address"
              required
            />
          </div>
          <div className="input-group">
            <label htmlFor="psw">Password</label>
            <input
              type="password"
              id="psw"
              placeholder="Create a password"
              required
            />
          </div>
          <div className="input-group">
            <label htmlFor="confirm-psw">Confirm Password</label>
            <input
              type="password"
              id="confirm-psw"
              placeholder="Confirm your password"
              required
            />
          </div>
          <div className="security-questions">
            <label htmlFor="security"></label>
          </div>
          <div className="actions">
            <button type="submit" className="btn register-btn">
              <Link to="/Add">Create Account</Link>
            </button>

            <Link to="/" className="btn cancel-btn">
              Cancel
            </Link>
          </div>
        </form>
      </div>
    </div>
  );
};

export default RegistrationPage;
