import React, { useState } from "react";
import { useNavigate } from "react-router-dom";
import { useAuth } from "../context/AuthContext";
import api from "../api";

export default function Register() {
  const navigate = useNavigate();
  const { login } = useAuth();
  const [formData, setFormData] = useState({
    name: "",
    email: "",
    password: "",
    confirmPassword: ""
  });
  const [error, setError] = useState("");
  const [loading, setLoading] = useState(false);

  const handleSubmit = async (e) => {
    e.preventDefault();
    setError("");

    // Validaciones
    if (formData.password !== formData.confirmPassword) {
      setError("Las contraseñas no coinciden");
      return;
    }

    if (formData.password.length < 6) {
      setError("La contraseña debe tener al menos 6 caracteres");
      return;
    }

    setLoading(true);

    try {
      // Datos para enviar al backend
      const registerData = {
        name: formData.name,
        email: formData.email,
        password: formData.password,
        // Algunos backends usan estos nombres:
        username: formData.email,
        first_name: formData.name.split(' ')[0],
        last_name: formData.name.split(' ').slice(1).join(' ') || '',
      };

      console.log("📤 Enviando registro:", registerData);

      // Intentar registro
      const response = await api.post("auth/register/", registerData);
      
      console.log("✅ Respuesta del servidor:", response.data);

      // Caso 1: El backend devuelve token y usuario directamente
      if (response.data.token && response.data.user) {
        localStorage.setItem("token", response.data.token);
        login(response.data.user);
        navigate("/dashboard");
      } 
      // Caso 2: El backend devuelve solo token (sin usuario)
      else if (response.data.token) {
        localStorage.setItem("token", response.data.token);
        // Crear objeto de usuario básico
        const user = {
          name: formData.name,
          email: formData.email,
          id: response.data.id || response.data.user_id
        };
        login(user);
        navigate("/dashboard");
      }
      // Caso 3: Solo crea el usuario, sin token
      else {
        console.log("✅ Usuario creado, redirigiendo a login");
        alert("Cuenta creada exitosamente. Por favor inicia sesión.");
        navigate("/login");
      }

    } catch (err) {
      console.error("❌ Error completo:", err);
      console.error("❌ Respuesta del error:", err.response?.data);
      
      // Manejo específico de errores
      if (err.response?.data) {
        const errorData = err.response.data;
        
        // Errores comunes de Django/DRF
        if (errorData.email) {
          setError(`Email: ${errorData.email[0] || errorData.email}`);
        } else if (errorData.password) {
          setError(`Contraseña: ${errorData.password[0] || errorData.password}`);
        } else if (errorData.username) {
          setError(`Usuario: ${errorData.username[0] || errorData.username}`);
        } else if (errorData.name) {
          setError(`Nombre: ${errorData.name[0] || errorData.name}`);
        } else if (errorData.message) {
          setError(errorData.message);
        } else if (errorData.detail) {
          setError(errorData.detail);
        } else if (errorData.error) {
          setError(errorData.error);
        } else {
          setError(`Error: ${JSON.stringify(errorData)}`);
        }
      } else if (err.request) {
        setError("No se pudo conectar con el servidor. Verifica tu conexión.");
      } else {
        setError("Error inesperado. Intenta nuevamente.");
      }
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-emerald-50 to-teal-50 flex items-center justify-center p-4">
      <div className="max-w-md w-full bg-white rounded-3xl shadow-xl p-8">
        <div className="text-center mb-8">
          <div className="w-20 h-20 bg-emerald-500 rounded-full flex items-center justify-center mx-auto mb-4 shadow-lg">
            <svg
              className="w-10 h-10 text-white"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M4.318 6.318a4.5 4.5 0 000 6.364L12 20.364l7.682-7.682a4.5 4.5 0 00-6.364-6.364L12 7.636l-1.318-1.318a4.5 4.5 0 00-6.364 0z"
              />
            </svg>
          </div>
          <h2 className="text-3xl font-bold text-gray-900">Crear cuenta</h2>
          <p className="text-gray-600 mt-2">Únete a MediCare</p>
        </div>

        {error && (
          <div className="mb-4 p-3 bg-red-100 border border-red-400 text-red-700 rounded-lg text-sm">
            {error}
          </div>
        )}

        <form onSubmit={handleSubmit} className="space-y-5">
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-2">
              Nombre completo
            </label>
            <input
              type="text"
              required
              value={formData.name}
              onChange={(e) => setFormData({...formData, name: e.target.value})}
              className="w-full px-4 py-3 rounded-xl border border-gray-300 focus:ring-2 focus:ring-emerald-500 focus:border-transparent outline-none transition"
              placeholder="Juan Pérez"
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-2">
              Correo electrónico
            </label>
            <input
              type="email"
              required
              value={formData.email}
              onChange={(e) => setFormData({...formData, email: e.target.value})}
              className="w-full px-4 py-3 rounded-xl border border-gray-300 focus:ring-2 focus:ring-emerald-500 focus:border-transparent outline-none transition"
              placeholder="tu@email.com"
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-2">
              Contraseña
            </label>
            <input
              type="password"
              required
              value={formData.password}
              onChange={(e) => setFormData({...formData, password: e.target.value})}
              className="w-full px-4 py-3 rounded-xl border border-gray-300 focus:ring-2 focus:ring-emerald-500 focus:border-transparent outline-none transition"
              placeholder="••••••••"
              minLength={6}
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-2">
              Confirmar contraseña
            </label>
            <input
              type="password"
              required
              value={formData.confirmPassword}
              onChange={(e) => setFormData({...formData, confirmPassword: e.target.value})}
              className="w-full px-4 py-3 rounded-xl border border-gray-300 focus:ring-2 focus:ring-emerald-500 focus:border-transparent outline-none transition"
              placeholder="••••••••"
              minLength={6}
            />
          </div>

          <button
            type="submit"
            disabled={loading}
            className="w-full bg-emerald-500 hover:bg-emerald-600 text-white font-semibold py-3 px-6 rounded-xl shadow-lg transition-all duration-200 transform hover:scale-105 disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {loading ? "Creando cuenta..." : "Registrarse"}
          </button>
        </form>

        <div className="mt-6 text-center space-y-3">
          <p className="text-gray-600">
            ¿Ya tienes cuenta?{" "}
            <button
              onClick={() => navigate("/login")}
              className="text-emerald-600 hover:text-emerald-700 font-medium"
            >
              Inicia sesión
            </button>
          </p>
          
          <button
            onClick={() => navigate("/")}
            className="text-gray-500 hover:text-gray-700 font-medium text-sm"
          >
            ← Volver al inicio
          </button>
        </div>
      </div>
    </div>
  );
}