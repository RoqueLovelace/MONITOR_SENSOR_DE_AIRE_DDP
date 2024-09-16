import cors from 'cors';

const corsOptions = {
  origin: '*', // Permitir todas las fuentes (puedes cambiarlo según necesites)
  methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'], // Métodos permitidos
  allowedHeaders: ['Content-Type', 'Authorization'], // Encabezados permitidos
  optionsSuccessStatus: 200 // Para navegadores antiguos
};

export const corsMiddleware = cors(corsOptions);
