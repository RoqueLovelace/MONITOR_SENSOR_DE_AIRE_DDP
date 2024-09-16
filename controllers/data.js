import { DataModel } from '../models/mysql/data.js';
import { ValidateParse } from '../schemes/data.js';

export class DataController {
  static async insertData (req, res) {
    const result = ValidateParse(req.body);
    if (!result.success) {
      const errorMessages = result.error.errors.map(err => err.message);
      return res.status(400).json({ er: errorMessages });
    }
    await DataModel.register({ data: result.data });
    res.status(201).send({ success: result.data });
  }
}
