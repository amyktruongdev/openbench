import React from 'react';
import { Link } from 'react-router-dom';

const Registration = () => {
  return (
    <div>
      <h1 className="header">OpenBench Create Account</h1>
      <div className="container">
        <label htmlFor="firstname"><b>First Name</b></label>
        <input type="text" placeholder="Enter Username" name="firstname" required />

        <label htmlFor="lastname"><b>Last Name</b></label>
        <input type="text" placeholder="Enter Password" name="lastname" required />

        <label htmlFor="uname"><b>Username</b></label>
        <input type="text" placeholder="Enter Password" name="uname" required />

        <label htmlFor="email"><b>Email</b></label>
        <input type="text" placeholder="Enter Password" name="email" required />

        <label htmlFor="psw"><b>Password</b></label>
        <input type="password" placeholder="Enter Password" name="psw" required />

        <label htmlFor="psw"><b>Password Confirmation</b></label>
        <input type="password" placeholder="Enter Password" name="psw" required />

        <button type="submit"><Link to="/Add">Create Account</Link></button>
      </div>

      <div className="container" style={{ backgroundColor: "#f1f1f1" }}>
        <button type="button" className="cancelbtn">Cancel</button>
        
      </div>
    </div>
  );
};

export default Registration;
