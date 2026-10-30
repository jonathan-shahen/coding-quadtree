// TODO: remove
#![allow(unused_variables)]

mod quadtree;

use std::io::{self, Write};

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

macro_rules! parse_or_continue {
    ($value:expr, $type:ty, $name:expr) => {
        match $value.parse::<$type>() {
            Ok(v) => v,
            Err(e) => {
                eprintln!("Invalid {}: {}", $name, e);
                continue;
            }
        }
    };
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();

    println!("Args: {}", args.csv_file);

    let quad_tree = QuadTree::from_csv(&args.csv_file)?;

    println!("");
    quad_tree.analyze()?;
    println!("");

    if args.early_quit {
        // Quit after building and analysing
        println!("EXITING: early exit parameter found");

        // TODO: Flush Tracy before exiting

        return Ok(());
    }

    let mut line = String::new();

    loop {
        print!("\nEnter lat,lon,radius,limit (or 'quit'): ");
        io::stdout().flush().unwrap();

        line.clear();
        if io::stdin().read_line(&mut line).is_err() {
            break;
        }

        let line = line.trim();

        if line == "quit" || line == "exit" {
            break;
        }
        let cols: Vec<&str> = line.split(',').collect();

        if cols.len() != 4 {
            eprintln!("Invalid format2");
            continue;
        }

        let lat = parse_or_continue!(cols[0], f32, "latitude");
        let lon = parse_or_continue!(cols[1], f32, "longitude");
        let radius = parse_or_continue!(cols[2], f32, "radius");
        let limit = parse_or_continue!(cols[3], usize, "limit");

        let result = quad_tree.find(lat, lon, radius, limit)?;

        let json = serde_json::to_string(&result)?;
        println!("{}", json);
    }

    Ok(())
}
