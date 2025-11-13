import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { useAuth } from "../context/AuthContext";

export default function Ajustes() {
  const navigate = useNavigate();
  const { user, logout, isAuthenticated } = useAuth();
  const [notificaciones, setNotificaciones] = useState(true);
  const [modoOscuro, setModoOscuro] = useState(false);

  // 🔒 Redirigir si no está autenticado
  useEffect(() => {
    if (!isAuthenticated) {
      navigate("/login");
    }
  }, [isAuthenticated, navigate]);

  const handleLogout = () => {
    logout();
    navigate("/");
  };

  const displayName =
    user?.name || user?.username || user?.email?.split("@")[0] || "Usuario";
  const displayEmail = user?.email || "correo@ejemplo.com";

  return (
    <div className="flex h-screen bg-gradient-to-br from-emerald-50 to-teal-50 overflow-hidden">
      {/* Sidebar */}
      <aside className="w-72 bg-gradient-to-b from-emerald-700 to-emerald-900 text-white p-6 flex flex-col h-screen">
        {/* Header */}
        <div className="mb-8">
          <div className="flex items-center gap-3 mb-2">
            <div className="w-10 h-10 bg-emerald-500 rounded-full flex items-center justify-center">
              <svg
                className="w-6 h-6"
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
            <h1 className="text-2xl font-bold">MediCare</h1>
          </div>
          <p className="text-emerald-200 text-sm ml-1">
            Gestión de medicamentos
          </p>
        </div>

        {/* Navigation */}
        <nav className="flex-1 space-y-2">
          <button
            onClick={() => navigate("/")}
            className="w-full flex items-center gap-3 px-4 py-3 rounded-xl hover:bg-emerald-800 transition-all"
          >
            <svg
              className="w-5 h-5"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M3 7v10a2 2 0 002 2h14a2 2 0 002-2V9a2 2 0 00-2-2h-6l-2-2H5a2 2 0 00-2 2z"
              />
            </svg>
            <span className="font-medium">Panel Principal</span>
          </button>

          <button
            onClick={() => navigate("/historial")}
            className="w-full flex items-center gap-3 px-4 py-3 rounded-xl hover:bg-emerald-800 transition-all"
          >
            <svg
              className="w-5 h-5"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M12 8v4l3 3m6-3a9 9 0 11-18 0 9 9 0 0118 0z"
              />
            </svg>
            <span className="font-medium">Historial</span>
          </button>

          <button
            onClick={() => navigate("/ajustes")}
            className="w-full flex items-center gap-3 px-4 py-3 rounded-xl bg-emerald-500 hover:bg-emerald-600 transition-all shadow-lg"
          >
            <svg
              className="w-5 h-5"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M10.325 4.317c.426-1.756 2.924-1.756 3.35 0a1.724 1.724 0 002.573 1.066c1.543-.94 3.31.826 2.37 2.37a1.724 1.724 0 001.065 2.572c1.756.426 1.756 2.924 0 3.35a1.724 1.724 0 00-1.066 2.573c.94 1.543-.826 3.31-2.37 2.37a1.724 1.724 0 00-2.572 1.065c-.426 1.756-2.924 1.756-3.35 0a1.724 1.724 0 00-2.573-1.066c-1.543.94-3.31-.826-2.37-2.37a1.724 1.724 0 00-1.065-2.572c-1.756-.426-1.756-2.924 0-3.35a1.724 1.724 0 001.066-2.573c-.94-1.543.826-3.31 2.37-2.37.996.608 2.296.07 2.572-1.065z"
              />
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M15 12a3 3 0 11-6 0 3 3 0 016 0z"
              />
            </svg>
            <span className="font-semibold">Ajustes</span>
          </button>
        </nav>

        {/* User Section */}
        <div className="mt-auto space-y-3">
          <div className="bg-emerald-600 rounded-xl p-4">
            <p className="text-sm text-emerald-200 mb-1">Usuario</p>
            <p className="font-semibold text-lg truncate">{displayName}</p>
          </div>

          <button
            onClick={handleLogout}
            className="w-full flex items-center justify-center gap-2 px-4 py-3 rounded-xl bg-red-500 hover:bg-red-600 transition-all font-semibold shadow-lg"
          >
            <svg
              className="w-5 h-5"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth={2}
                d="M17 16l4-4m0 0l-4-4m4 4H7m6 4v1a3 3 0 01-3 3H6a3 3 0 01-3-3V7a3 3 0 013-3h4a3 3 0 013 3v1"
              />
            </svg>
            Cerrar Sesión
          </button>
        </div>
      </aside>

      {/* Main Content */}
      <main className="flex-1 overflow-y-auto">
        <div className="p-8">
          <div className="max-w-4xl mx-auto">
            {/* Header con botón volver */}
            <div className="mb-8">
              <button
                onClick={() => navigate("/")}
                className="flex items-center gap-2 text-blue-600 hover:text-blue-700 font-medium mb-4 transition-colors"
              >
                <svg
                  className="w-5 h-5"
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                >
                  <path
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    strokeWidth={2}
                    d="M15 19l-7-7 7-7"
                  />
                </svg>
                Volver
              </button>
              <h2 className="text-4xl font-bold text-gray-900">Ajustes</h2>
            </div>

            {/* Información Personal */}
            <div className="bg-white rounded-2xl shadow-lg p-8 mb-6">
              <h3 className="text-2xl font-bold text-gray-900 mb-6">
                Información Personal
              </h3>
              <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
                <div>
                  <label className="block text-sm font-medium text-gray-600 mb-2">
                    Nombre
                  </label>
                  <div className="bg-gray-50 rounded-xl px-4 py-3 text-gray-800 font-medium">
                    {displayName}
                  </div>
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-600 mb-2">
                    Correo
                  </label>
                  <div className="bg-gray-50 rounded-xl px-4 py-3 text-gray-800 font-medium">
                    {displayEmail}
                  </div>
                </div>
              </div>
            </div>

            {/* Preferencias */}
            <div className="bg-white rounded-2xl shadow-lg p-8 mb-6">
              <h3 className="text-2xl font-bold text-gray-900 mb-6">
                Preferencias
              </h3>
              <div className="space-y-4">
                {/* Notificaciones */}
                <div className="flex items-center justify-between py-4 border-b border-gray-200">
                  <div>
                    <p className="text-lg font-semibold text-gray-800">
                      Notificaciones
                    </p>
                    <p className="text-sm text-gray-600">
                      Recibe recordatorios para tomar tus medicamentos
                    </p>
                  </div>
                  <button
                    onClick={() => setNotificaciones(!notificaciones)}
                    className={`relative inline-flex h-8 w-14 items-center rounded-full transition-colors ${
                      notificaciones ? "bg-blue-600" : "bg-gray-300"
                    }`}
                  >
                    <span
                      className={`inline-block h-6 w-6 transform rounded-full bg-white transition-transform ${
                        notificaciones ? "translate-x-7" : "translate-x-1"
                      }`}
                    />
                  </button>
                </div>

                {/* Modo Oscuro */}
                <div className="flex items-center justify-between py-4">
                  <div>
                    <p className="text-lg font-semibold text-gray-800">
                      Modo oscuro
                    </p>
                    <p className="text-sm text-gray-600">
                      Cambia la apariencia de la aplicación
                    </p>
                  </div>
                  <button
                    onClick={() => setModoOscuro(!modoOscuro)}
                    className={`relative inline-flex h-8 w-14 items-center rounded-full transition-colors ${
                      modoOscuro ? "bg-blue-600" : "bg-gray-300"
                    }`}
                  >
                    <span
                      className={`inline-block h-6 w-6 transform rounded-full bg-white transition-transform ${
                        modoOscuro ? "translate-x-7" : "translate-x-1"
                      }`}
                    />
                  </button>
                </div>
              </div>
            </div>

            {/* Botón Cerrar Sesión */}
            <button
              onClick={handleLogout}
              className="w-full flex items-center justify-center gap-2 bg-red-500 hover:bg-red-600 text-white font-semibold px-6 py-4 rounded-2xl shadow-lg transition-all"
            >
              <svg
                className="w-5 h-5"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  strokeLinecap="round"
                  strokeLinejoin="round"
                  strokeWidth={2}
                  d="M17 16l4-4m0 0l-4-4m4 4H7m6 4v1a3 3 0 01-3 3H6a3 3 0 01-3-3V7a3 3 0 013-3h4a3 3 0 013 3v1"
                />
              </svg>
              Cerrar sesión
            </button>
          </div>
        </div>
      </main>
    </div>
  );
}