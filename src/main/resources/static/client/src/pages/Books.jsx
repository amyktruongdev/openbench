import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { Link } from 'react-router-dom';

const Books = () => {
    const [books, setBooks] = useState([]);

    useEffect(() => {
        const fetchAllBooks = async () => {
            try {
                const res = await axios.get("http://localhost:8800/books");
                setBooks(res.data);
            } catch (err) {
                console.error("Error fetching books:", err);
            }
        };
        fetchAllBooks();
    }, []);

    return (
        <div>
            <h1>OpenBench</h1>
            <div className="books">
                {books.map((book) => (
                    <div className="book" key={book.idbooks}>
                        {book.cover && <img src={book.cover} alt={book.title} />}
                        <h2>{book.title}</h2>
                        <p>{book.desc}</p>
                        <span>${book.price}</span>
                    </div>
                ))}
            </div>
            <Link to="/add" className="add-button">
                Add New Book
            </Link>
        </div>
    );
};

export default Books;
