use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Default, Debug)]
pub struct SearchResults {
    /// Ids that were found within the Radius
    pub found_ids: Vec<i64>,
    /// Ids that were found within the 2*Radius, excluding the  */
    pub extra_ids: Vec<i64>,
}
