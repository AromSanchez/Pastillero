import React from "react";
import { useNavigate } from "react-router-dom";

export default function Welcome() {
  const navigate = useNavigate();

  return (
    <div className="min-h-screen bg-gradient-to-br from-emerald-50 to-teal-50 flex items-center justify-center p-4">
      <div className="max-w-md w-full text-center">
        {/* Logo */}
        <div className="mb-8 flex justify-center">
          <div className="w-32 h-32 bg-emerald-500 rounded-full flex items-center justify-center shadow-xl">
            <svg
              className="w-16 h-16 text-white"
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
        </div>

        {/* Título */}
        <h1 className="text-5xl font-bold text-gray-900 mb-4">MediCare</h1>
        
        {/* Subtítulo */}
        <p className="text-lg text-gray-600 mb-12">
          Tu asistente confiable para gestionar tus medicamentos diarios
        </p>

        {/* Botones */}
        <div className="space-y-4">
          <button
            onClick={() => navigate("/login")}
            className="w-full bg-emerald-500 hover:bg-emerald-600 text-white font-semibold py-4 px-6 rounded-2xl shadow-lg transition-all duration-200 transform hover:scale-105"
          >
            Iniciar sesión
          </button>

          <button
            onClick={() => navigate("/register")}
            className="w-full bg-white hover:bg-gray-50 text-emerald-600 font-semibold py-4 px-6 rounded-2xl border-2 border-emerald-500 transition-all duration-200 transform hover:scale-105"
          >
            Registrarse
          </button>
        </div>
      </div>
    </div>
  );
}