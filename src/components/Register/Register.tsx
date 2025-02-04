/* eslint-disable @typescript-eslint/no-unused-vars */
import { useState } from "react";

function Register() {
  const [userName, setUserName] = useState("");
  const [validName, setValidName] = useState(false);

  const [pwd, setPwd] = useState("");
  const [validPwd, setValidPwd] = useState(false);

  const [confirmPwd, setConfirmPwd] = useState('');
  const [pwdMatch, setPwdMatch] = useState(false);

  

  return (
    <>
      <div className="container">
        {/* Include Format Action */}
        <form action="">
          <h1 style={{ textAlign: "center" }}>Register</h1>
          <div className="form-group">
            <input
              id="username"
              type="text"
              className="form-control"
              placeholder="Username"
            />
            <br></br>
          </div>

          <div className="form-group">
            <input
              id="pwd"
              type="password"
              className="form-control"
              placeholder="Password"
            />
            <br></br>
          </div>
          <div className="form-group">
            <input
              id="pwd-confirm"
              type="password"
              className="form-control"
              placeholder="Confirm Password"
            />
            <br></br>
            <button className="btn btn-primary">Submit</button>
          </div>
        </form>
        <a href="#">Login</a>
      </div>
    </>
  );
}

export default Register;
