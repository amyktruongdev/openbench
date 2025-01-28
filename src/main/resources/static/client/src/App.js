import React from 'react';
import {BrowserRouter, Link, Route, Routes} from "react-router-dom";
import Books from "./pages/Books";
import Add from "./pages/Add";
import Update from "./pages/Update";
import Forgot from "./pages/Forgot";
import Registration from "./pages/Registration";

function App() {
    return (
        <div className="App">
            <BrowserRouter>
                <nav>
                    <Link to="/">Books</Link> |
                    <Link to="/add">Add</Link> |
                    <Link to="/update">Update</Link> |
                    <Link to="/forgot">Forgot Password</Link> |
                    <Link to="/registration">Register</Link>
                </nav>
                <Routes>
                    <Route path="/" element={<Books />} />
                    <Route path="/add" element={<Add />} />
                    <Route path="/update" element={<Update />} />
                    <Route path="/forgot" element={<Forgot />} />
                    <Route path="/registration" element={<Registration />} />
                    <Route path="*" element={<h1>404: Page Not Found</h1>} />
                </Routes>
            </BrowserRouter>
        </div>
    );
}

export default App;
