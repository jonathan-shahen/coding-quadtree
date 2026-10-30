use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Default, Debug)]
pub struct SearchResults {
    /// Ids that were found within the Radius
    found_ids: Vec<i64>,
    /// Ids that were found within the 2*Radius, excluding the  */
    extra_ids: Vec<i64>,
}
