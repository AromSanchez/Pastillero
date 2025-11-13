import { BrowserRouter, Routes, Route } from "react-router-dom";
import Welcome from "./pages/Welcome";
import Login from "./pages/Login";
import Register from "./pages/Register";
import Home from "./pages/Home";
import EditarCompartimento from "./pages/EditarCompartimento";
import Historial from "./pages/Historial";
import Ajustes from "./pages/Ajustes"; // ✅ AÑADIDO
import './App.css';

function App() {
  return (
    <BrowserRouter>
      <Routes>
        {/* Pantalla de bienvenida */}
        <Route path="/" element={<Welcome />} />
        
        {/* Autenticación */}
        <Route path="/login" element={<Login />} />
        <Route path="/register" element={<Register />} />
        
        {/* Dashboard principal */}
        <Route path="/dashboard" element={<Home />} />
        
        {/* Edición de compartimentos */}
        <Route path="/editar/:id" element={<EditarCompartimento />} />
        <Route path="/editar/nuevo/:compartimento" element={<EditarCompartimento />} />

        {/* Historial */}
        <Route path="/historial" element={<Historial />} />
        
        {/* ✅ Ajustes */}
        <Route path="/ajustes" element={<Ajustes />} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;