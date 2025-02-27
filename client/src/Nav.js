function Nav() {
    return (
      <div className="navbar">
        <ul>
          <li><a className="active" href="/Landing">Dashboard</a></li>
          <li><a href="/maintenance">Maintenance</a></li>
          <li><a href="/analytics.html">Analytics</a></li>
          <li><a href="/map">Map</a></li>
          <li><a href="#">Settings</a></li>
          <li><a href="/login">Logout</a></li>
        </ul>
      </div>
    );
  }

  export default Nav;
  