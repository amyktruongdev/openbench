import "./App.css";
import Footer from "./components/Footer/Footer";
import Login from "./components/Login/Login";
import NavBar from "./components/NavBar/NavBar";
import Register from "./components/Register/Register";

function App() {
  return (
    <>
      <NavBar></NavBar>
      <h1>Hello World!</h1>
      <Login ></Login>
      <br></br>
      <Register></Register>
      <Footer></Footer>
    </>
  );
}

export default App;
