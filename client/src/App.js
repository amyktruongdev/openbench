import {
  BrowserRouter,
  Routes,
  Route,
} from "react-router-dom";
import Landing from "./pages/Landing";
import Books from "./pages/Books";
import Login from "./pages/Login";
import Forgot from "./pages/Forgot";
import Registration from "./pages/Registration";
import Map from "./pages/Map";
import Maintenance from "./pages/Maintenance";


function App() {
  return (
    <div className="App">
      <BrowserRouter>
      <Routes>
        <Route path="/" element={<Books/>}/>
        <Route path="/landing" element={<Landing/>}/>
        <Route path="/login" element={<Login/>}/>
        <Route path="/forgot" element={<Forgot/>}/>
        <Route path="/registration" element={<Registration/>}/>
        <Route path="/map" element={<Map/>}/>
      </Routes>
      </BrowserRouter>

    </div>
  );
}

export default App;
