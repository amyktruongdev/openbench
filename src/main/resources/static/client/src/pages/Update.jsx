import React, { useState } from 'react';
import { Link } from 'react-router-dom';

const Update = () => {
    const [credentials, setCredentials] = useState({ username: '', password: '' });

    const handleChange = (e) => {
        const { name, value } = e.target;
        setCredentials({ ...credentials, [name]: value });
    };

    const handleSubmit = (e) => {
        e.preventDefault();
        // Handle login logic (e.g., API call)
        console.log('Logging in with:', credentials);
    };

    return (
        <div>
            <h1 className="header">OpenBench Login</h1>
            <form className="container" onSubmit={handleSubmit}>
                <label htmlFor="uname"><b>Username</b></label>
                <input
                    type="text"
                    placeholder="Enter Username"
                    name="username"
                    value={credentials.username}
                    onChange={handleChange}
                    required
                />

                <label htmlFor="psw"><b>Password</b></label>
                <input
                    type="password"
                    placeholder="Enter Password"
                    name="password"
                    value={credentials.password}
                    onChange={handleChange}
                    required
                />

                <button type="submit" className="loginbtn">Login</button>
                <label>
                    <input type="checkbox" defaultChecked name="remember" /> Remember me
                </label>
            </form>

            <div className="container" style={{ backgroundColor: "#f1f1f1" }}>
                <Link to="/" className="cancelbtn">Cancel</Link>
                <span className="psw">
          Forgot <Link to="/forgot">password?</Link>
        </span>
                <Link to="/registration" className="createbtn">Create Account</Link>
            </div>
        </div>
    );
};

export default Update;
