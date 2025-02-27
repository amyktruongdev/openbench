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
              minlength="5"
              maxlength="12"
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
              minlength="8"
              maxlength="16" 
            />
             {/*One upper, one lower, one special character for password*/}
          </div>
          <div className="input-group">
            <label htmlFor="confirm-psw">Confirm Password</label>
            <input
              type="password"
              id="confirm-psw"
              placeholder="Confirm your password"
              required
              minlength="5"
              maxlength="12"
            />
          </div>
          <div className="input-group">
            <label htmlFor="security">Security Question</label>
            <select name="securityq" id="securityq" required>
              <option value="" disabled selected>Select Security Question</option>
              <option value="sq1">Name of first ever pet</option>
              <option value="sq2">Model of your first car</option>
              <option value="sq3">Your mothers surname</option>
              <option value="sq4">Street name you live on</option>
            </select>
            {/*Answer section to send to backend*/}
          </div>
          <div className="actions">
            <button type="submit" className="btn register-btn">
              <Link to="/Landing">Create Account</Link>
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
