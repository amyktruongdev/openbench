import React, { useState } from 'react';

const Forgot = () => {
    const [email, setEmail] = useState('');
    const [message, setMessage] = useState('');

    const handleSubmit = (e) => {
        e.preventDefault();
        // Simulate sending the email for password recovery
        setMessage(`If an account with ${email} exists, password reset instructions have been sent.`);
    };

    return (
        <div className="forgot-container">
            <h1 className="header">Forgot Password</h1>
            <p>Enter your email address to reset your password:</p>
            <form onSubmit={handleSubmit} className="forgot-form">
                <input
                    type="email"
                    placeholder="Enter your email"
                    value={email}
                    onChange={(e) => setEmail(e.target.value)}
                    required
                />
                <button type="submit">Send Reset Instructions</button>
            </form>
            {message && <p className="message">{message}</p>}
        </div>
    );
};

export default Forgot;
