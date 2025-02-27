import {
  BrowserRouter,
  Routes,
  Route,
} from "react-router-dom";
import Add from "./pages/Add";
import Books from "./pages/Books";
import Update from "./pages/Update";
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
        <Route path="/add" element={<Add/>}/>
        <Route path="/update" element={<Update/>}/>
        <Route path="/forgot" element={<Forgot/>}/>
        <Route path="/registration" element={<Registration/>}/>
        <Route path="/map" element={<Map/>}/>
      </Routes>
      </BrowserRouter>

    </div>
  );
}

export default App;
