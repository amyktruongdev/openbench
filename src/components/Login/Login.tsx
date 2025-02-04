/* eslint-disable @typescript-eslint/no-unused-vars */
import { useState } from "react";

function Login() {

  const [userName, setUserName] = useState('');
  const [validName, setValidName] = useState(false);

  const [pwd, setPwd] = useState('');
  const [validPwd, setValidPwd] = useState(false);



    return (
        <>
             <div className="container">
          {/* Include Format Action */}
          <form action="">
              <h1 style={{textAlign: "center"}}>Login</h1>
              <div className="form-group">
                <input id="username" type="text" className="form-control" placeholder="Username"/>
                <br></br>
              </div>
              
              <div className="form-group">
                <input id="pwd" type="password" className="form-control" placeholder="Password"/>
                <br></br>
              </div>
                <br></br>
                <button className="btn btn-primary">Submit</button>
          </form>
          <a href="#">New User?</a>
        </div>
        </>
    )
}

export default Login;