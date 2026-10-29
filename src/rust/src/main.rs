mod quadtree;

use clap::Parser;

use quadtree::QuadTree;

/// QuadTree rust implementation
#[derive(Parser, Debug)]
#[command(version, about)]
struct Args {
    /// CSV filepath 
    csv_file: String,

    /// Number of times to greet
    #[arg(short = 'q', default_value_t = false)]
    early_quit: bool,
}

fn main() {
    let args = Args::parse();
    
    println!("Args: {}", args.csv_file);

    let mut quad_tree = QuadTree::from_csv(&args.csv_file);
}
