import express, { json } from 'express';
import cors from 'cors';
const app = express();
const PORT = process.env.PORT ?? 1234;

app.disable('x-powered-by');

app.use(cors());
app.use(json());

app.post('/post', (req, res) => {
  const data = req.body;
  console.log(data);
  res.status(200).send(data);
});

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost/${PORT}`);
});
