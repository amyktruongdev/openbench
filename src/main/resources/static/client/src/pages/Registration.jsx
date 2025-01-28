import React, { useState } from 'react';
import { Link } from 'react-router-dom';

const Registration = () => {
    const [formData, setFormData] = useState({
        firstname: '',
        lastname: '',
        username: '',
        email: '',
        password: '',
        confirmPassword: ''
    });

    const handleChange = (e) => {
        const { name, value } = e.target;
        setFormData({ ...formData, [name]: value });
    };

    const handleSubmit = (e) => {
        e.preventDefault();
        if (formData.password !== formData.confirmPassword) {
            alert("Passwords do not match!");
            return;
        }
        // Handle form submission logic (e.g., API call)
        console.log('Form Data Submitted:', formData);
    };

    return (
        <div>
            <h1 className="header">OpenBench Create Account</h1>
            <form className="container" onSubmit={handleSubmit}>
                <label htmlFor="firstname"><b>First Name</b></label>
                <input
                    type="text"
                    placeholder="Enter First Name"
                    name="firstname"
                    value={formData.firstname}
                    onChange={handleChange}
                    required
                />

                <label htmlFor="lastname"><b>Last Name</b></label>
                <input
                    type="text"
                    placeholder="Enter Last Name"
                    name="lastname"
                    value={formData.lastname}
                    onChange={handleChange}
                    required
                />

                <label htmlFor="username"><b>Username</b></label>
                <input
                    type="text"
                    placeholder="Enter Username"
                    name="username"
                    value={formData.username}
                    onChange={handleChange}
                    required
                />

                <label htmlFor="email"><b>Email</b></label>
                <input
                    type="email"
                    placeholder="Enter Email"
                    name="email"
                    value={formData.email}
                    onChange={handleChange}
                    required
                />

                <label htmlFor="password"><b>Password</b></label>
                <input
                    type="password"
                    placeholder="Enter Password"
                    name="password"
                    value={formData.password}
                    onChange={handleChange}
                    required
                />

                <label htmlFor="confirmPassword"><b>Password Confirmation</b></label>
                <input
                    type="password"
                    placeholder="Confirm Password"
                    name="confirmPassword"
                    value={formData.confirmPassword}
                    onChange={handleChange}
                    required
                />

                <button type="submit" className="create-account-btn">Create Account</button>
            </form>

            <div className="container" style={{ backgroundColor: "#f1f1f1" }}>
                <Link to="/" className="cancelbtn">Cancel</Link>
            </div>
        </div>
    );
};

export default Registration;
