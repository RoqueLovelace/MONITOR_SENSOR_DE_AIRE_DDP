import { DataModel } from '../models/mysql/data.js';
import { ValidateParse } from '../schemes/data.js';

export class DataController {
  static async insertData (req, res) {
    const result = ValidateParse(req.body);
    if (!result.success) {
      const errorMessages = result.error.errors.map(err => err.message);
      return res.status(400).json({ er: errorMessages });
    }

    try {
      await DataModel.register({ data: result.data });
      console.log(result.data);
      res.status(201).send({ success: result.data });
    } catch (error) {
      console.error('Error inserting data:', error);
      res.status(500).json({ error: 'Error inserting data' });
    }
  }

  static async getAll (req, res) {
    const { fecha } = req.query;

    try {
      const data = await DataModel.getAll({ fecha });
      res.json(data);
    } catch (error) {
      console.error('Error retrieving data:', error);
      res.status(500).json({ error: 'Error retrieving data' });
    }
  }
}
