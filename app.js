import express, { json } from 'express';
import { DataController } from './controllers/data.js';
import { corsMiddleware } from './middleware/cors.js';
const app = express();
const PORT = process.env.PORT ?? 1234;

app.disable('x-powered-by');

app.use(corsMiddleware);
app.use(json());

app.post('/post', DataController.insertData);

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
