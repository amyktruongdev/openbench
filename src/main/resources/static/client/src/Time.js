import React, { useState, useEffect } from 'react';

function DateTime() {
  const [dateTime, setDateTime] = useState(new Date());

  useEffect(() => {
    const intervalId = setInterval(() => {
      setDateTime(new Date());
    }, 1000); // Update every second

    return () => clearInterval(intervalId); // Cleanup on unmount
  }, []);

  return (
    <div className="datetime-container">
      <div className="datetime">
        {dateTime.toLocaleString()}
      </div>
    </div>
  );
}

export default DateTime;
