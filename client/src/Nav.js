function Nav() {
    return (
      <div className="navbar">
        <ul>
          <li><a className="active" href="index.html">Dashboard</a></li>
          <li><a href="/maintenance.html">Maintenance</a></li>
          <li><a href="/analytics.html">Analytics</a></li>
          <li><a href="#">Settings</a></li>
          <li><a href="/update">Logout</a></li>
        </ul>
      </div>
    );
  }

  export default Nav;
  