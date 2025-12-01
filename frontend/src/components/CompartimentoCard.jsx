import React, { useState } from "react";
import { Link } from "react-router-dom";
import api from "../api";

const colors = [
  { hex: "#ffb3b3", name: "Rojo", ring: "#ff6b6b" },
  { hex: "#ffe4a3", name: "Amarillo", ring: "#ffc107" },
  { hex: "#a8e6cf", name: "Verde", ring: "#4caf50" },
  { hex: "#a3c9ff", name: "Azul", ring: "#2196f3" }
];

const EditIcon = () => (
  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
    <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
    <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
  </svg>
);

const TrashIcon = () => (
  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
    <polyline points="3 6 5 6 21 6"></polyline>
    <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path>
  </svg>
);

export default function CompartimentoCard({ compartimento, tratamiento, onDelete }) {
  const [showDeleteModal, setShowDeleteModal] = useState(false);
  const [deleting, setDeleting] = useState(false);

  const colorData = colors[compartimento - 1];

  const handleDelete = async () => {
    if (!tratamiento) return;
    
    setDeleting(true);
    try {
      await api.delete(`tratamientos/${tratamiento.id}/`);
      setShowDeleteModal(false);
      if (onDelete) {
        onDelete();
      }
    } catch (error) {
      console.error("Error al eliminar tratamiento:", error);
      alert("Error al eliminar el tratamiento");
    } finally {
      setDeleting(false);
    }
  };

  return (
    <>
      <div
        className="rounded-3xl p-6 shadow-md border-4"
        style={{ 
          backgroundColor: colorData.hex,
          borderColor: colorData.ring
        }}
      >
        {/* Header */}
        <div className="flex justify-between items-center mb-6">
          <div className="bg-white rounded-full px-4 py-2 shadow-sm">
            <span className="text-gray-800 font-semibold">Compartimento {compartimento}</span>
          </div>
          <div className="flex gap-3">
            <Link
              to={tratamiento ? `/editar/${tratamiento.id}` : `/editar/nuevo/${compartimento}`}
              className="bg-white p-3 rounded-xl shadow-sm hover:shadow-md transition-shadow text-green-600"
            >
              <EditIcon />
            </Link>
            {tratamiento && (
              <button 
                onClick={() => setShowDeleteModal(true)}
                className="bg-white p-3 rounded-xl shadow-sm hover:shadow-md transition-shadow text-red-600"
              >
                <TrashIcon />
              </button>
            )}
          </div>
        </div>

        {/* Content */}
        {tratamiento ? (
          <div className="space-y-4">
            {/* Pastilla */}
            <div>
              <p className="text-gray-700 text-sm mb-1 font-medium">Pastilla</p>
              <p className="text-gray-900 text-2xl font-bold">{tratamiento.nombre_pastilla}</p>
            </div>

            {/* Grid de información */}
            <div className="grid grid-cols-3 gap-4">
              <div>
                <p className="text-gray-700 text-sm mb-1 font-medium">Dosis</p>
                <p className="text-gray-900 text-3xl font-bold">{tratamiento.dosis}</p>
              </div>
              <div>
                <p className="text-gray-700 text-sm mb-1 font-medium">Stock</p>
                <p className="text-gray-900 text-3xl font-bold">{tratamiento.stock}</p>
              </div>
              <div>
                <p className="text-gray-700 text-sm mb-1 font-medium">Repetición</p>
                <p className="text-gray-900 text-xl font-bold">{tratamiento.repeticion}</p>
              </div>
            </div>

            {/* Hora o información adicional */}
            {tratamiento.repeticion === "DIARIO" && (
              <div>
                <p className="text-gray-700 text-sm mb-1 font-medium">Hora</p>
                <p className="text-gray-900 text-3xl font-bold">{tratamiento.hora_toma}</p>
              </div>
            )}
            
            {tratamiento.repeticion === "SEMANAL" && (
              <div>
                <p className="text-gray-700 text-sm mb-1 font-medium">Día</p>
                <p className="text-gray-900 text-2xl font-bold">
                  {["Lun","Mar","Mié","Jue","Vie","Sáb","Dom"][tratamiento.dia_semana]}
                </p>
                <p className="text-gray-700 text-sm mb-1 mt-3 font-medium">Hora</p>
                <p className="text-gray-900 text-3xl font-bold">{tratamiento.hora_toma}</p>
              </div>
            )}
            
            {tratamiento.repeticion === "CADA_X_HORAS" && (
              <div>
                <p className="text-gray-700 text-sm mb-1 font-medium">Cada</p>
                <p className="text-gray-900 text-3xl font-bold">{tratamiento.intervalo_horas} horas</p>
              </div>
            )}
          </div>
        ) : (
          <div className="text-center py-8">
            <p className="text-gray-800 text-xl font-semibold">No configurado</p>
          </div>
        )}
      </div>

      {/* Modal de Confirmación */}
      {showDeleteModal && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
          <div className="bg-white rounded-2xl p-6 max-w-md w-full mx-4 shadow-2xl">
            <div className="text-center mb-6">
              <div className="w-16 h-16 bg-red-100 rounded-full flex items-center justify-center mx-auto mb-4">
                <svg className="w-8 h-8 text-red-600" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16" />
                </svg>
              </div>
              <h3 className="text-xl font-bold text-gray-900 mb-2">
                ¿Eliminar medicamento?
              </h3>
              <p className="text-gray-600">
                ¿Estás seguro de eliminar <strong>{tratamiento?.nombre_pastilla}</strong> del compartimento {compartimento}? Esta acción no se puede deshacer.
              </p>
            </div>

            <div className="flex gap-3">
              <button
                onClick={() => setShowDeleteModal(false)}
                disabled={deleting}
                className="flex-1 px-4 py-3 rounded-xl bg-gray-200 hover:bg-gray-300 text-gray-800 font-semibold transition-all disabled:opacity-50"
              >
                Cancelar
              </button>
              <button
                onClick={handleDelete}
                disabled={deleting}
                className="flex-1 px-4 py-3 rounded-xl bg-red-500 hover:bg-red-600 text-white font-semibold transition-all disabled:opacity-50 flex items-center justify-center gap-2"
              >
                {deleting ? (
                  <>
                    <div className="w-5 h-5 border-2 border-white border-t-transparent rounded-full animate-spin"></div>
                    Eliminando...
                  </>
                ) : (
                  "Eliminar"
                )}
              </button>
            </div>
          </div>
        </div>
      )}
    </>
  );
}